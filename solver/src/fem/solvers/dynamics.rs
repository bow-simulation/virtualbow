use std::fmt::{Debug, Display, Formatter};
use nalgebra::{DMatrix, DVector};
use crate::fem::system::system::{System, DynamicEval};
use crate::numerics::newton;
use crate::numerics::newton::solve_newton;

#[derive(Copy, Clone)]
pub struct Settings {
    pub timestep: TimeStep,    // Time step settings
    pub epsilon_rel: f64,      // Relative tolerance
    pub epsilon_abs: f64,      // Absolute tolerance
    pub max_iterations: u32,   // Maximum number of iterations per load step
    pub max_stagnation: u32    // Maximum number of iterations that don't improve the objective
}

#[derive(Copy, Clone)]
pub enum TimeStep {
    Fixed(f64),                    // Use a fixed timestep during the simulation
    Adaptive{
        min_timestep: f64,         // Minimum allowed timestep to ensure that the solver finishes in a reasonable amount of time
        max_timestep: f64,         // Maximum allowed timestep to ensure a certain resolution of the solution
        steps_per_period: usize    // Target number of timesteps per current characteristic period
    }
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            timestep: TimeStep::default(),
            epsilon_rel: 1e-08,
            epsilon_abs: 1e-10,
            max_iterations: 50,
            max_stagnation: 10
        }
    }
}

impl Default for TimeStep {
    fn default() -> Self {
        /*
        Self::Adaptive {
            min_time_step: 1e-6,
            max_time_step: 1e-3,
            steps_per_period: 50,
        }
        */

        Self::Fixed(1e-6)
    }
}

#[derive(PartialEq, Debug, Copy, Clone)]
pub enum DynamicSolverError {
    EquilibriumError(newton::Error),    // Error during the equilibrium iterations
    SingularMassMatrix,       // The mass matrix is singular, i.e. cannot be inverted
    AbortedByCaller           // Aborted by the callback function's return value  // TODO: Add separate end condition function in addition to the callback in order to detect this
}

impl Display for DynamicSolverError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            DynamicSolverError::EquilibriumError(error) => Display::fmt(&error, f)?,
            DynamicSolverError::SingularMassMatrix      => write!(f, "The system has a singular mass matrix.")?,
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

        // The time step is either set to a fixed value or, in the case of adaptive stepping, to the minimum threshold.
        let mut dt = match self.settings.timestep {
            TimeStep::Fixed(dt) => dt,
            TimeStep::Adaptive{ min_timestep, .. } => min_timestep
        };

        loop {
            // Current displacements, velocities and accelerations
            let u_current = self.system.get_displacements().clone();
            let v_current = self.system.get_velocities().clone();
            let a_current = eval.get_accelerations().clone();

            let mut u_next = DVector::<f64>::zeros(u_current.len());
            let mut v_next = DVector::<f64>::zeros(u_current.len());

            // Residuum function
            // Input: Accelerations at next time point
            // Output: Residuum and its jacobian at next time point
            let residuum = |a_next: &DVector<f64>, r_next: &mut DVector<f64>, drda: &mut DMatrix<f64>| {
                v_next.copy_from(&(&v_current + dt*(1.0 - gamma)*&a_current + dt*gamma*a_next));
                u_next.copy_from(&(&u_current + dt*&v_current + dt*dt*((0.5 - beta)*&a_current + beta*a_next)));

                self.system.set_time(t + dt);
                self.system.set_displacements(&u_next);
                self.system.set_velocities(&v_next);
                self.system.eval_dynamics(&mut eval);

                r_next.copy_from(&(eval.get_mass_matrix().component_mul(&a_next) + eval.get_internal_forces() - eval.get_external_forces()));
                drda.copy_from(&(DMatrix::<f64>::from_diagonal(eval.get_mass_matrix()) + dt*gamma*eval.get_damping_matrix() + dt*dt*beta*eval.get_stiffness_matrix()));
            };

            solve_newton(residuum, a_current.clone(), newton::Settings::default()).map_err(|e| DynamicSolverError::EquilibriumError(e))?;

            t += dt;

            if !callback(&self.system, &eval) {
                return Ok(());
            }

            // Determine the next timestep in case of adaptive time stepping
            if let TimeStep::Adaptive { min_timestep, max_timestep, steps_per_period, .. } = self.settings.timestep {
                // Displacement increment during the last time step
                let delta_u = &u_next - &u_current;

                // Current characteristic frequency
                let num: f64 = delta_u.dot(&(eval.get_stiffness_matrix()*&delta_u));
                let den: f64 = delta_u.dot(&(eval.get_mass_matrix().component_mul(&delta_u)));
                let omega = f64::sqrt(f64::abs(num/den));

                let dt_suggested = std::f64::consts::TAU/(omega*(steps_per_period as f64));    // Timestep suggested by the number of steps per characteristic period
                let dt_suggested = f64::min(dt_suggested/dt, 2.0)*dt;                          // Limit relative increase with respect to the previous timestep
                dt = num::clamp(dt_suggested, min_timestep, max_timestep);                     // Limit absolute value to minimum and maximum settings
            }
        }
    }
}