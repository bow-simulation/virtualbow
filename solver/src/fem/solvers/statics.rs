use nalgebra::DVector;
use crate::fem::system::system::{System, StaticEval};

use iter_num_tools::lin_space;
use crate::fem::system::dof::Dof;

#[derive(Copy, Clone)]
pub struct Settings {
    pub epsilon_rel: f64,       // Relative tolerance
    pub epsilon_abs: f64,       // Absolute tolerance
    pub full_update: bool,      // Whether to update the stiffness matrix in each iteration
    pub line_search: bool,      // Whether to perform a line search in each iteration
    pub max_iterations: u32,    // Maximum number of iterations per load step
    pub max_stagnation: u32     // Maximum number of iterations that don't improve the objective
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            epsilon_rel: 1e-08,
            epsilon_abs: 1e-10,
            full_update: true,
            line_search: false,
            max_iterations: 50,
            max_stagnation: 10
        }
    }
}

#[derive(Debug)]
pub struct IterationInfo {
    pub iterations: u32,    // Number of iterations performed
}

#[derive(PartialEq, Debug)]
pub enum IterationError {
    LinearSolutionFailed,     // Decomposition of the tangent stiffness matrix or solution of the linear system failed
    InvalidConstraintEval,    // The return values of the constraint function don't allow for a solution of the problem
    MaxIterationsReached,     // Maximum number of iterations was reached without convergence
    MaxStagnationReached,     // Maximum number of iterations without improvement was reached
}

#[derive(PartialEq, Debug)]
pub enum ContinuationError {
    IterationFailed(f64, IterationError),
    AbortedByCaller
}

pub struct StaticSolver<'a> {
    system: &'a mut System,
    settings: Settings,
    pub statics: StaticEval  // TODO: Make private and deal with issues
}

impl<'a> StaticSolver<'a> {
    pub fn new(system: &'a mut System, settings: Settings) -> Self {
        let statics = system.create_static_eval();
        Self {
            system,
            settings,
            statics
        }
    }

    // Solves for equilibrium of the system with an additional constraint
    // on the displacement vector u and the load factor lambda.
    pub fn solve_equilibrium_constrained<F>(&mut self, mut constraint: F) -> Result<IterationInfo, IterationError>
        where F: FnMut(&DVector<f64>, f64, &mut DVector<f64>, &mut f64) -> f64
    {
        // Set velocities to zero and copy the current displacement vector
        self.system.set_velocities(&DVector::zeros(self.system.n_dofs()));
        let mut u = self.system.get_displacements().clone();

        let mut min_error_u = f64::INFINITY;
        let mut min_error_λ = f64::INFINITY;
        let mut stagnations = 0_u32;

        let mut dcdu = DVector::<f64>::zeros(self.system.n_dofs());
        let mut dcdλ = 0.0;

        for i in 1..=self.settings.max_iterations {
            // Apply state to system and elements and evaluate internal forces and tangent stiffness matrix
            self.system.set_displacements(&u);
            self.system.eval_statics(&mut self.statics);

            // Factorize stiffness matrix, calculate out of balance loads and auxiliary vectors alpha and beta
            let decomposition = self.statics.get_tangent_stiffness_matrix().clone().lu();

            let delta_q = self.statics.get_load_factor()*self.statics.get_unscaled_external_forces() - self.statics.get_internal_forces();
            let alpha = decomposition.solve(&delta_q).ok_or(IterationError::LinearSolutionFailed)?;
            let beta = decomposition.solve(&self.statics.get_unscaled_external_forces()).ok_or(IterationError::LinearSolutionFailed)?;

            // Evaluate constraint and calculate change in load parameter and displacement
            let c = constraint(&u, self.statics.get_load_factor(), &mut dcdu, &mut dcdλ);
            let delta_λ = -(c + dcdu.dot(&alpha))/(dcdλ + dcdu.dot(&beta));
            let delta_u = &alpha + delta_λ *&beta;

            // Check if the load factor increment is valid
            // The denominator might have been zero, pointing to an ill-posed problem
            if !f64::is_finite(delta_λ) {
                return Err(IterationError::InvalidConstraintEval);
            }

            // Check convergence criteria, return if fulfilled
            let abs_error_u = delta_u.amax();
            let abs_error_λ = delta_λ.abs();

            let stopping_criterion_u = abs_error_u < self.settings.epsilon_rel*u.amax() + self.settings.epsilon_abs;
            let stopping_criterion_λ = abs_error_λ < self.settings.epsilon_rel*self.statics.get_load_factor().abs() + self.settings.epsilon_abs;
            if stopping_criterion_u && stopping_criterion_λ {
                return Ok(IterationInfo{ iterations: i });
            }

            // Check if any of the errors has been decreased
            // If yes, record the new minimum and reset the stagnation counter
            // If no, increase the stagnation counter and return error if the limit is reached
            if (abs_error_u < min_error_u) || (abs_error_λ < min_error_λ) {
                min_error_u = abs_error_u;
                min_error_λ = abs_error_λ;
                stagnations = 0;
            }
            else {
                stagnations += 1;
                if stagnations == self.settings.max_stagnation {
                    return Err(IterationError::MaxStagnationReached);
                }
            }

            // Apply changes to load factor and displacements
            self.statics.set_load_factor(self.statics.get_load_factor() + delta_λ);
            u += &delta_u;
        }

        // Maximum number of iterations exceeded
        return Err(IterationError::MaxIterationsReached);
    }

    // Solve for equilibrium of the system with a load constraint in the form of a given load factor
    pub fn solve_equilibrium_load_controlled(&mut self, λ_target: f64) -> Result<IterationInfo, IterationError> {
        self.solve_equilibrium_constrained(|_u, lambda, dcdu, dcdλ| {
            dcdu.fill(0.0);
            *dcdλ = 1.0;

            return lambda - λ_target;
        })
    }

    // Solve for equilibrium of the system with a displacement constraint in the form of a given target displacement for a dof
    pub fn solve_equilibrium_displacement_controlled(&mut self, dof: Dof, u_target: f64) -> Result<IterationInfo, IterationError> {
        match dof {
            Dof::Free(i) => {
                return self.solve_equilibrium_constrained(|u, _lambda, dcdu, dcdλ| {
                    dcdu.fill(0.0);
                    dcdu[i] = 1.0;
                    *dcdλ = 0.0;

                    return u[i] - u_target;
                });
            },
            Dof::Fixed(_) => {
                panic!("Can't perform displacement control on a fixed dof");
            }
        }
    }

    // Callback: (system) -> ()
    pub fn equilibrium_path_load_controlled<F>(&mut self, steps: usize, callback: F) -> Result<(), ContinuationError>
        where F: Fn(&System, &StaticEval) -> bool
    {
        // If the number of intermediate load steps is zero, perform only one solution for lambda = 1.
        // Otherwise divide the range lambda = [0, 1] into the required number of steps and solve each point.
        if steps == 0 {
            self.solve_equilibrium_load_controlled(1.0).map_err(|e| ContinuationError::IterationFailed(1.0, e))?;
            if !callback(self.system, &self.statics) {
                return Err(ContinuationError::AbortedByCaller)
            }
        }
        else {
            for lambda in lin_space(0.0..=1.0, steps - 1) {
                self.solve_equilibrium_load_controlled(lambda).map_err(|e| ContinuationError::IterationFailed(lambda, e))?;
                if !callback(self.system, &self.statics) {
                    return Err(ContinuationError::AbortedByCaller)
                }
            }
        }

        return Ok(());
    }

    // Callback: (system) -> ()
    pub fn equilibrium_path_displacement_controlled<F>(&mut self, dof: Dof, target: f64, steps: usize, callback: &mut F) -> Result<(), ContinuationError>
        where F: FnMut(&System, &StaticEval) -> bool
    {
        // If the number of intermediate load steps is zero, perform only one solution for lambda = 1.
        // Otherwise divide the range lambda = [0, 1] into the required number of steps and solve each point.
        if steps == 0 {
            self.solve_equilibrium_displacement_controlled(dof, target).map_err(|e| ContinuationError::IterationFailed(target, e))?;
            if !callback(self.system, &self.statics) {
                return Err(ContinuationError::AbortedByCaller)
            }
        }
        else {
            for displacement in lin_space(self.system.get_displacement(dof)..=target, steps + 1) {
                self.solve_equilibrium_displacement_controlled(dof, displacement).map_err(|e| ContinuationError::IterationFailed(target, e))?;
                if !callback(self.system, &self.statics) {
                    return Err(ContinuationError::AbortedByCaller)
                }
            }
        }

        return Ok(());
    }
}