use std::fmt::{Display, Formatter};
use nalgebra::{DMatrix, DVector};
use crate::fem::system::system::{System, StaticEval};

use iter_num_tools::lin_space;
use crate::fem::system::dof::Dof;
use crate::numerics::newton::{solve_newton, solve_newton_constrained, IterationInfo, NewtonSettings, NewtonError};

#[derive(PartialEq, Debug)]
pub enum StaticSolverError {
    EquilibriumError(NewtonError),    // Error during the equilibrium iterations
    AbortedByCaller           // Aborted by the callback function's return value
}

impl Display for StaticSolverError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            StaticSolverError::EquilibriumError(error) => Display::fmt(&error, f)?,
            StaticSolverError::AbortedByCaller         => write!(f, "Aborted by the caller.")?,
        }

        Ok(())
    }
}

impl std::error::Error for StaticSolverError {

}

pub struct StaticSolver<'a> {
    system: &'a mut System,
    settings: NewtonSettings,
    eval: StaticEval
}

impl<'a> StaticSolver<'a> {
    pub fn new(system: &'a mut System, settings: NewtonSettings) -> Self {
        let eval = system.create_static_eval();
        Self {
            system,
            settings,
            eval
        }
    }

    // Solve for equilibrium of the system with a load constraint in the form of a given load factor
    pub fn equilibrium_load_controlled(&mut self, λ_target: f64) -> Result<IterationInfo, StaticSolverError> {
        self.system.set_velocities(&DVector::zeros(self.system.n_dofs()));
        self.eval.set_load_factor(λ_target);

        let x0 = self.system.get_displacements().clone();

        let mut f = |x: &DVector<f64>, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>| {
            self.system.set_displacements(&x);
            self.system.eval_statics(&mut self.eval);

            f.copy_from(&(self.eval.get_internal_forces() - self.eval.get_load_factor()*self.eval.get_unscaled_external_forces()));
            dfdx.copy_from(self.eval.get_tangent_stiffness_matrix());
        };

        solve_newton(&mut f, x0, self.settings)
            .map_err(|e| StaticSolverError::EquilibriumError(e))
    }

    // points = steps + 1
    pub fn equilibrium_path_load_controlled<F>(&mut self, steps: usize, callback: &mut F) -> Result<(), StaticSolverError>
        where F: FnMut(&System, &StaticEval) -> bool
    {
        // If the number of intermediate load steps is zero, perform only one solution for lambda = 1.
        // Otherwise divide the range lambda = [0, 1] into the required number of steps and solve each point.
        if steps == 0 {
            self.equilibrium_load_controlled(1.0)?;
            if !callback(self.system, &self.eval) {
                return Err(StaticSolverError::AbortedByCaller)
            }
        }
        else {
            for lambda in lin_space(0.0..=1.0, steps + 1) {
                self.equilibrium_load_controlled(lambda)?;
                if !callback(self.system, &self.eval) {
                    return Err(StaticSolverError::AbortedByCaller)
                }
            }
        }

        return Ok(());
    }

    // Solve for equilibrium of the system with a displacement constraint in the form of a given target displacement for a dof
    pub fn equilibrium_displacement_controlled(&mut self, dof: Dof, u_target: f64) -> Result<IterationInfo, StaticSolverError> {
        // Extract index of the controlled displacement from the dof
        let index = match dof {
            Dof::Free(i) => i,
            Dof::Fixed(_) => panic!("Can't perform displacement control on a fixed dof")
        };

        self.system.set_velocities(&DVector::zeros(self.system.n_dofs()));

        let x0 = self.system.get_displacements().clone();
        let λ0 = self.eval.get_load_factor();

        let mut f = |x: &DVector<f64>, λ: f64, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>, dfdλ: &mut DVector<f64>| {
            self.system.set_displacements(&x);
            self.eval.set_load_factor(λ);
            self.system.eval_statics(&mut self.eval);

            f.copy_from(&(self.eval.get_internal_forces() - self.eval.get_load_factor()*self.eval.get_unscaled_external_forces()));
            dfdx.copy_from(self.eval.get_tangent_stiffness_matrix());
            dfdλ.copy_from(&(-self.eval.get_unscaled_external_forces()));
        };

        let mut c = |x: &DVector<f64>, _λ: f64, c: &mut f64, dcdx: &mut DVector<f64>, dcdλ: &mut f64| {
            *c = x[index] - u_target;
            *dcdλ = 0.0;

            dcdx.fill(0.0);
            dcdx[index] = 1.0;
        };

        solve_newton_constrained(&mut f, &mut c, x0, λ0, self.settings)
            .map_err(|e| StaticSolverError::EquilibriumError(e))
    }
    
    // points = steps + 1
    pub fn equilibrium_path_displacement_controlled<F>(&mut self, dof: Dof, target: f64, steps: usize, callback: &mut F) -> Result<(), StaticSolverError>
        where F: FnMut(&System, &StaticEval) -> bool
    {
        // If the number of intermediate load steps is zero, perform only one solution for lambda = 1.
        // Otherwise divide the range lambda = [0, 1] into the required number of steps and solve each point.
        if steps == 0 {
            self.equilibrium_displacement_controlled(dof, target)?;
            if !callback(self.system, &self.eval) {
                return Err(StaticSolverError::AbortedByCaller)
            }
        }
        else {
            for displacement in lin_space(self.system.get_displacement(dof)..=target, steps + 1) {
                self.equilibrium_displacement_controlled(dof, displacement)?;
                if !callback(self.system, &self.eval) {
                    return Err(StaticSolverError::AbortedByCaller)
                }
            }
        }

        return Ok(());
    }
}