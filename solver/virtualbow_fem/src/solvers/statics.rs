use std::fmt::{Display, Formatter};
use nalgebra::{DMatrix, DVector};
use crate::system::system::{SolverEval, System, SystemEval};

use iter_num_tools::lin_space;
use virtualbow_num::newton::{solve_newton, solve_newton_constrained, NewtonError, NewtonInfo, NewtonSettings, NewtonTolerances};
use crate::system::dof::{Dof, DofDimension};

#[derive(Copy, Clone, Debug)]
pub struct StaticTolerances {
    pub linear_pos: f64,     // Absolute tolerance for lengths
    pub angular_pos: f64,    // Absolute tolerance for angles
    pub loadfactor: f64      // Absolute tolerance for load factor
}

impl StaticTolerances {
    // Constructs absolute tolerances from reference values for positions and angles and a relative tolerance
    pub fn new(ref_linear_pos: f64, ref_angular_pos: f64, relative_tolerance: f64) -> Self {
        Self {
            linear_pos: ref_linear_pos*relative_tolerance,
            angular_pos: ref_angular_pos*relative_tolerance,
            loadfactor: relative_tolerance
        }
    }

    // Determine tolerances for the system's displacement vector, taking into account the dimensions
    // of the system dofs and the corresponding tolerances
    pub fn xtol(&self, system: &System) -> DVector<f64> {
        DVector::<f64>::from_fn(system.n_dofs(), |i, _| {
            match system.get_dimensions()[i] {
                DofDimension::Position => self.linear_pos,
                DofDimension::Rotation => self.angular_pos
            }
        })
    }
}

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

// Stores the current state of the system, so that it can be restored in case the solution fails.
fn save_state(system: &System) -> (DVector<f64>, DVector<f64>) {
    (system.get_displacements().clone(), system.get_velocities().clone())
}

// Restores the system state if there has been a failure
fn restore_state_on_failure(system: &mut System, result: &Result<NewtonInfo, StaticSolverError>, u0: &DVector<f64>, v0: &DVector<f64>) {
    if let Err(StaticSolverError::EquilibriumError(_)) = result {
        system.set_state(u0, v0);
    }
}

pub struct LoadControl<'a> {
    system: &'a mut System,
    tolerances: StaticTolerances,
    settings: NewtonSettings,
}

impl<'a> LoadControl<'a> {
    pub fn new(system: &'a mut System, tolerances: StaticTolerances, settings: NewtonSettings) -> Self {
        Self {
            system,
            tolerances,
            settings
        }
    }

    // Static equilibrium at full external forces, no intermediate steps
    pub fn solve_equilibrium(self) -> Result<NewtonInfo, StaticSolverError> {
        self.solve_equilibrium_path(1, &mut |_, _, _| true)
    }

    // Static equilibrium for load factors from 0 to 1 with a given number of steps
    // points = steps + 1, callback evaluated at each point
    //
    // If the equilibrium iterations fail, the system is restored to the state it had before the
    // call, such that a failed attempt has no observable effect. If the caller aborts the solution
    // instead, the system is left at the last equilibrium state that was reached.
    pub fn solve_equilibrium_path<F>(mut self, steps: usize, callback: &mut F) -> Result<NewtonInfo, StaticSolverError>
        where F: FnMut(&SystemEval, &SolverEval, &NewtonInfo) -> bool    // TODO: struct StepInfo { index, lambda }?
    {
        let (u0, v0) = save_state(self.system);
        let result = self.solve_equilibrium_path_impl(steps, callback);
        restore_state_on_failure(self.system, &result, &u0, &v0);

        result
    }

    fn solve_equilibrium_path_impl<F>(&mut self, steps: usize, callback: &mut F) -> Result<NewtonInfo, StaticSolverError>
        where F: FnMut(&SystemEval, &SolverEval, &NewtonInfo) -> bool
    {
        assert!(steps >= 1, "At least one step is required");

        let n = self.system.n_dofs();

        let mut p0 = DVector::zeros(n);
        let mut pλ = DVector::zeros(n);

        let mut q = DVector::zeros(n);
        let mut K = DMatrix::zeros(n, n);
        let a = DVector::zeros(n);    // Accelerations stay zero, only used as a result

        // The full/unscaled external loads have to be calculated only once
        self.system.compute_external_forces(&mut p0);

        // Velocities are zero in a static solution. The state is applied once here so that the
        // system is consistent before the first equilibrium iteration, and the same zero vector
        // is then handed to every evaluation below.
        let v_static = DVector::zeros(n);
        let u_current = self.system.get_displacements().clone();
        self.system.set_state(&u_current, &v_static);

        // Determine Newton tolerances for the system dofs according to their dimensions
        // and the corresponding static tolerances
        let tolerances = NewtonTolerances {
            xtol: self.tolerances.xtol(self.system),
            λtol: 0.0 // Irrelevant for unconstrained problem
        };

        // Track current newton iteration info
        let mut info: NewtonInfo = NewtonInfo::default();

        // Compute an equilibrium state for each load factor from 0 to 1
        for λ in lin_space(0.0..=1.0, steps + 1) {
            // Compute current scaled load
            pλ.copy_from(&(λ*&p0));

            // Initial guess for Newton iterations
            let x0 = self.system.get_displacements().clone();

            // Objective function for static equilibrium
            let mut objective = |x: &DVector<f64>, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>| {
                self.system.evaluate(x, &v_static, Some(&mut q), Some(&mut K), None);

                f.copy_from(&(&q - &pλ));    // Residual forces with scaled external loads
                dfdx.copy_from(&K);          // Jacobian given by tangent stiffness matrix
            };

            // Perform Newton iterations to find equilibrium
            info = solve_newton(&mut objective, &x0, &tolerances, &self.settings).map_err(StaticSolverError::EquilibriumError)?;

            // Execute callback and pass current system info
            if !callback(&self.system.evaluated(), &SolverEval::new(&pλ, &q, &a), &info) {
                return Err(StaticSolverError::AbortedByCaller)
            }
        }

        Ok(info)
    }
}

pub struct DisplacementControl<'a> {
    system: &'a mut System,
    tolerances: StaticTolerances,
    settings: NewtonSettings,
}

impl<'a> DisplacementControl<'a> {
    pub fn new(system: &'a mut System, tolerances: StaticTolerances, settings: NewtonSettings) -> Self {
        Self {
            system,
            tolerances,
            settings
        }
    }

    // Static equilibrium at full external forces, no intermediate steps
    pub fn solve_equilibrium(self, dof: Dof, u_target: f64) -> Result<NewtonInfo, StaticSolverError> {
        self.solve_equilibrium_path(dof, u_target, 1, &mut |_, _, _| true)
    }

    // Static equilibrium for target displacement from current to given value with a given number of steps
    // points = steps + 1, callback evaluated at each point
    //
    // If the equilibrium iterations fail, the system is restored to the state it had before the
    // call, such that a failed attempt has no observable effect. If the caller aborts the solution
    // instead, the system is left at the last equilibrium state that was reached.
    pub fn solve_equilibrium_path<F>(mut self, dof: Dof, u_target: f64, steps: usize, callback: &mut F) -> Result<NewtonInfo, StaticSolverError>
    where F: FnMut(&SystemEval, &SolverEval, &NewtonInfo) -> bool    // TODO: struct StepInfo { index, lambda }?
    {
        let (u0, v0) = save_state(self.system);
        let result = self.solve_equilibrium_path_impl(dof, u_target, steps, callback);
        restore_state_on_failure(self.system, &result, &u0, &v0);

        result
    }

    fn solve_equilibrium_path_impl<F>(&mut self, dof: Dof, u_target: f64, steps: usize, callback: &mut F) -> Result<NewtonInfo, StaticSolverError>
    where F: FnMut(&SystemEval, &SolverEval, &NewtonInfo) -> bool
    {
        assert!(dof.is_active(), "Controlled dof must be active");
        assert!(steps >= 1, "At least one step is required");

        let n = self.system.n_dofs();

        let mut p0 = DVector::zeros(n);
        let mut pλ = DVector::zeros(n);
        let mut λi = 0.0;    // TODO: Is zero always the best initial value? Depends on whether the system is currently in equilibrium or not.

        let mut q = DVector::zeros(n);
        let mut K = DMatrix::zeros(n, n);
        let a = DVector::zeros(n);    // Accelerations stay zero, only used as a result

        // The full/unscaled external loads have to be calculated only once
        self.system.compute_external_forces(&mut p0);

        // Velocities are zero in a static solution. The state is applied once here so that the
        // system is consistent before the first equilibrium iteration, and the same zero vector
        // is then handed to every evaluation below.
        let v_static = DVector::zeros(n);
        let u_current = self.system.get_displacements().clone();
        self.system.set_state(&u_current, &v_static);

        // Determine tolerances for the Newton method
        let tolerances = NewtonTolerances {
            xtol: self.tolerances.xtol(self.system),
            λtol: self.tolerances.loadfactor
        };

        // Track current newton iteration info
        let mut info: NewtonInfo = NewtonInfo::default();

        // Compute an equilibrium state for each displacement from current state to target
        for u_target in lin_space(self.system.get_dof_displacement(dof)..=u_target, steps + 1) {
            // Initial values of displacements and load factor
            let u0 = self.system.get_displacements().clone();
            let λ0 = 1.0;

            // Objective function for static equilibrium
            let mut objective = |u: &DVector<f64>, λ: f64, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>, dfdλ: &mut DVector<f64>| {
                // Apply displacements to the system
                self.system.evaluate(u, &v_static, Some(&mut q), Some(&mut K), None);

                // Apply load scaling
                λi = λ;
                pλ = λ*&p0;

                // Compute residual forces and derivatives
                f.copy_from(&(&q - &pλ));
                dfdx.copy_from(&K);
                dfdλ.copy_from(&(-&p0));
            };

            // Constraint function for displacement control
            let mut constraint = |u: &DVector<f64>, _λ: f64, c: &mut f64, dcdx: &mut DVector<f64>, dcdλ: &mut f64| {
                *c = u[dof.index] - u_target;
                *dcdλ = 0.0;

                dcdx.fill(0.0);
                dcdx[dof.index] = 1.0;
            };

            info = solve_newton_constrained(&mut objective, &mut constraint, u0, λ0, &tolerances, &self.settings)
                .map_err(StaticSolverError::EquilibriumError)?;

            // Execute callback and pass current system info
            if !callback(&self.system.evaluated(), &SolverEval::new(&pλ, &q, &a), &info) {    // TODO: Pass complete info to caller? Might be a cleaner API.
                return Err(StaticSolverError::AbortedByCaller)
            }
        }

        Ok(info)
    }
}