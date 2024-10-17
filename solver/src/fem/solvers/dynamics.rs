use std::fmt::{Display, Formatter};
use nalgebra::{DMatrix, DVector};
use crate::fem::system::system::{System, DynamicEval};

use super::eigen::natural_frequencies_from_matrices;

#[derive(Copy, Clone)]
pub struct Settings {
    pub timestep: f64,          // Time step
    pub epsilon_rel: f64,       // Relative tolerance
    pub epsilon_abs: f64,       // Absolute tolerance
    pub max_iterations: u32,    // Maximum number of iterations per load step
    pub max_stagnation: u32     // Maximum number of iterations that don't improve the objective
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            timestep: 1e-6,
            epsilon_rel: 1e-08,
            epsilon_abs: 1e-10,
            max_iterations: 50,
            max_stagnation: 10
        }
    }
}

#[derive(PartialEq, Debug)]
pub enum DynamicSolverError {
    SingularMassMatrix,       // The mass matrix is singular, i.e. cannot be inverted
    LinearSolutionFailed,     // Decomposition of the tangent stiffness matrix or solution of the linear system failed
    NonFiniteStateIncrement,  // The displacement delta is not finite, i.e. contains NaN or Inf values
    MaxIterationsReached,     // Maximum number of iterations was reached without convergence
    MaxStagnationReached,     // Maximum number of iterations without improvement was reached
    AbortedByCaller           // Aborted by the callback function's return value  // TODO: Add separate end condition function in addition to the callback in order to detect this
}

impl Display for DynamicSolverError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            DynamicSolverError::SingularMassMatrix      => write!(f, "The system has a singular mass matrix.")?,
            DynamicSolverError::LinearSolutionFailed    => write!(f, "Decomposition/solution of the tangent stiffness matrix failed.")?,
            DynamicSolverError::NonFiniteStateIncrement => write!(f, "Encountered a non-finite displacement increment.")?,
            DynamicSolverError::MaxIterationsReached    => write!(f, "Maximum number of iterations exceeded.")?,
            DynamicSolverError::MaxStagnationReached    => write!(f, "Maximum number of stagnating iterations exceeded.")?,
            DynamicSolverError::AbortedByCaller         => write!(f, "Aborted by the caller.")?,
        }

        Ok(())
    }
}

impl std::error::Error for DynamicSolverError {

}

pub struct DynamicSolver<'a> {
    system: &'a mut System,
    settings: Settings
}

impl<'a> DynamicSolver<'a> {
    pub fn new(system: &'a mut System, settings: Settings) -> Self {
        Self {
            system,
            settings
        }
    }

    pub fn solve<F>(&mut self, callback: &mut F) -> Result<(), DynamicSolverError>
        where F: FnMut(&System, &DynamicEval) -> bool
    {
        // Constant average acceleration (unconditionally stable for linear systems)
        let beta  = 0.25;
        let gamma = 0.50;

        // Dynamic system state
        let mut eval = self.system.create_dynamic_eval();

        // Solver state
        let mut t = self.system.get_time();

        // Check if the mass matrix is positive definite
        if eval.get_mass_matrix().amax() < 0.0 {
            return Err(DynamicSolverError::SingularMassMatrix);
        }

        // First evaluation at start of the simulated interval to make acceleration available and provide callback information at t = t0.
        self.system.eval_dynamics(&mut eval);
        if !callback(&self.system, &eval) {
            return Ok(());
        }

        let dt = self.settings.timestep;

        println!("");

        loop {
            // Current displacements, velocities and accelerations
            let u_current = self.system.get_displacements().clone();
            let v_current = self.system.get_velocities().clone();
            let a_current = eval.get_accelerations().clone();

            let mut a_next = a_current.clone();  // DVector::zeros(n);
            let mut v_next = &v_current + dt*(1.0 - gamma)*&a_current + dt*gamma*&a_next;
            let mut u_next = &u_current + dt*&v_current + dt*dt*((0.5 - beta)*&a_current + beta*&a_next);

            let mut min_error_a = f64::INFINITY;
            let mut stagnations = 0_u32;

            // Next acceleration to iterate on
            for i in 0..self.settings.max_iterations {
                self.system.set_time(t + dt);
                self.system.set_displacements(&u_next);
                self.system.set_velocities(&v_next);
                self.system.eval_dynamics(&mut eval);

                let r: DVector<f64> = eval.get_external_forces() - eval.get_internal_forces() - eval.get_mass_matrix().component_mul(&a_next);
                let drda: DMatrix<f64> = DMatrix::<f64>::from_diagonal(eval.get_mass_matrix()) + dt*gamma*eval.get_damping_matrix() + dt*dt*beta*eval.get_stiffness_matrix();

                let decomposition = drda.lu();
                let delta_a = decomposition.solve(&r).ok_or(DynamicSolverError::LinearSolutionFailed)?;

                // Check if the acceleration increment is finite
                if !delta_a.iter().cloned().all(f64::is_finite) {
                    return Err(DynamicSolverError::NonFiniteStateIncrement);
                }

                // Check for convergence
                let abs_error_a = delta_a.amax();
                if abs_error_a < self.settings.epsilon_rel*a_next.amax() + self.settings.epsilon_abs {
                    break;
                }

                // Check for number of iterations
                if i == self.settings.max_iterations - 1 {
                    return Err(DynamicSolverError::MaxIterationsReached);
                }

                // Check if the error has been decreased
                // If yes, record the new minimum and reset the stagnation counter
                // If no, increase the stagnation counter and return error if the limit is reached
                if abs_error_a < min_error_a {
                    min_error_a = abs_error_a;
                    stagnations = 0;
                }
                else {
                    stagnations += 1;
                    if stagnations == self.settings.max_stagnation {
                        return Err(DynamicSolverError::MaxStagnationReached);
                    }
                }

                a_next += &delta_a;
                v_next += dt*gamma*&delta_a;
                u_next += dt*dt*beta*&delta_a;
            }

            t += dt;

            // Eigenfrequencies at the end of the timestep

            let delta_u = &u_next - &u_current;

            let num: f64 = delta_u.dot(&(eval.get_stiffness_matrix()*&delta_u));
            let den: f64 = delta_u.dot(&(eval.get_mass_matrix().component_mul(&delta_u)));

            let n_steps_per_period = 50;

            let omega = f64::sqrt(f64::abs(num/den));
            println!("{}, {}, {}", t, omega, std::f64::consts::TAU/(omega*(n_steps_per_period as f64)));

            if !callback(&self.system, &eval) {
                return Ok(());
            }
        }
    }
}