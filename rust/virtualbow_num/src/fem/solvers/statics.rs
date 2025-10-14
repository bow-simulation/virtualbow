use std::fmt::{Display, Formatter};
use nalgebra::{DMatrix, DVector};
use crate::fem::system::system::{System, SystemEval};

use iter_num_tools::lin_space;
use crate::fem::system::dof::Dof;
use crate::utils::newton::{solve_newton, solve_newton_constrained, IterationResult, NewtonSettings, NewtonError};

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
    λ: f64,              // Load scaling factor
    p0: DVector<f64>,    // Unscaled external forces
    pλ: DVector<f64>,    // Scaled external forces
    q: DVector<f64>,
    a: DVector<f64>,
    K: DMatrix<f64>,
}

impl<'a> StaticSolver<'a> {
    pub fn new(system: &'a mut System, settings: NewtonSettings) -> Self {
        let n = system.n_dofs();

        // The unscaled external loads have to be calculated only once
        let mut p0 = DVector::zeros(n);
        system.compute_external_forces(&mut p0);

        Self {
            system,
            settings,
            λ: 1.0,
            p0,
            pλ: DVector::zeros(n),
            q: DVector::zeros(n),
            a: DVector::zeros(n),
            K: DMatrix::zeros(n, n),
        }
    }

    // Solve for equilibrium of the system with a load constraint in the form of a given load factor
    pub fn equilibrium_load_controlled(&mut self, λ: f64) -> Result<IterationResult, StaticSolverError> {
        self.system.set_velocities(&DVector::zeros(self.system.n_dofs()));
        let u0 = self.system.get_displacements().clone();

        self.pλ = λ*&self.p0;

        let mut f = |x: &DVector<f64>, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>| {
            self.system.set_displacements(x);
            self.system.compute_internal_forces(Some(&mut self.q), Some(&mut self.K), None);

            f.copy_from(&(&self.q - &self.pλ));
            dfdx.copy_from(&self.K);
        };

        solve_newton(&mut f, u0, self.settings).map_err(StaticSolverError::EquilibriumError)
    }

    // points = steps + 1
    pub fn equilibrium_path_load_controlled<F>(&mut self, steps: usize, callback: &mut F) -> Result<(), StaticSolverError>
        where F: FnMut(&System, &SystemEval) -> bool    // TODO: struct StepInfo { index, lambda }?
    {
        // If the number of intermediate load steps is zero, perform only one solution for lambda = 1.
        // Otherwise divide the range lambda = [0, 1] into the required number of steps and solve each point.
        if steps == 0 {
            self.equilibrium_load_controlled(1.0)?;
            if !callback(self.system, &SystemEval::new(&self.pλ, &self.q, &self.a)) {
                return Err(StaticSolverError::AbortedByCaller)
            }
        }
        else {
            for lambda in lin_space(0.0..=1.0, steps + 1) {
                self.equilibrium_load_controlled(lambda)?;
                if !callback(self.system, &SystemEval::new(&self.pλ, &self.q, &self.a)) {
                    return Err(StaticSolverError::AbortedByCaller)
                }
            }
        }

        Ok(())
    }

    // Solve for equilibrium of the system with a displacement constraint in the form of a given target displacement for a dof
    pub fn equilibrium_displacement_controlled(&mut self, dof: Dof, u_target: f64) -> Result<IterationResult, StaticSolverError> {
        assert!(dof.is_active(), "Can't perform displacement control on a locked dof");

        self.system.set_velocities(&DVector::zeros(self.system.n_dofs()));
        let x0 = self.system.get_displacements().clone();
        let λ0 = self.λ;

        let mut f = |x: &DVector<f64>, λ: f64, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>, dfdλ: &mut DVector<f64>| {
            self.system.set_displacements(x);
            self.system.compute_internal_forces(Some(&mut self.q), Some(&mut self.K), None);

            self.λ = λ;
            self.pλ = self.λ*&self.p0;

            f.copy_from(&(&self.q - &self.pλ));
            dfdx.copy_from(&self.K);
            dfdλ.copy_from(&(-&self.p0));
        };

        let mut c = |x: &DVector<f64>, _λ: f64, c: &mut f64, dcdx: &mut DVector<f64>, dcdλ: &mut f64| {
            *c = x[dof.index] - u_target;
            *dcdλ = 0.0;

            dcdx.fill(0.0);
            dcdx[dof.index] = 1.0;
        };

        solve_newton_constrained(&mut f, &mut c, x0, λ0, self.settings)
            .map_err(StaticSolverError::EquilibriumError)
    }
    
    // points = steps + 1
    pub fn equilibrium_path_displacement_controlled<F>(&mut self, dof: Dof, u_target: f64, steps: usize, callback: &mut F) -> Result<(), StaticSolverError>
        where F: FnMut(&System, &SystemEval, f64) -> bool    // Last argument is the stiffness of the force-displacement relationship  // TODO: struct StepInfo { index, lambda, stiffness }?
    {
        assert!(dof.is_active(), "Can't perform displacement control on a locked dof");

        // If the number of intermediate load steps is zero, perform only one solution for lambda = 1.
        // Otherwise divide the range lambda = [0, 1] into the required number of steps and solve each point.
        // TODO: Code duplication in two cases below
        if steps == 0 {
            let info = self.equilibrium_displacement_controlled(dof, u_target)?;
            if !callback(self.system, &SystemEval::new(&self.pλ, &self.q, &self.a), 1.0/info.dxdλ[dof.index]) {
                return Err(StaticSolverError::AbortedByCaller)
            }
        }
        else {
            for displacement in lin_space(self.system.get_displacement(dof)..=u_target, steps + 1) {
                let info = self.equilibrium_displacement_controlled(dof, displacement)?;
                if !callback(self.system, &SystemEval::new(&self.pλ, &self.q, &self.a), 1.0/info.dxdλ[dof.index]) {
                    return Err(StaticSolverError::AbortedByCaller)
                }
            }
        }

        Ok(())
    }
}