use std::fmt::{Debug, Display, Formatter};
use nalgebra::{DMatrix, DVector};
use crate::fem::system::dof::Dof;
use crate::fem::system::system::{System, SystemEval};
use crate::utils::newton::{NewtonError, NewtonSettings, solve_newton, solve_newton_constrained};

#[derive(Copy, Clone)]
pub struct DynamicSolverSettings {
    pub time_stepping: TimeStepping,    // Time step settings
    pub max_time: f64,                  // Maximum time after which the simulation is aborted if no regular stopping condition was met
    pub newton: NewtonSettings,         // Settings for Newton iteration
}

impl Default for DynamicSolverSettings {
    fn default() -> Self {
        Self  {
            time_stepping: TimeStepping::Fixed(1e-6),
            max_time: f64::INFINITY,
            newton: Default::default(),
        }
    }
}

#[derive(Copy, Clone)]
pub enum TimeStepping {
    Fixed(f64),                    // Use a fixed timestep during the simulation
    Adaptive{
        min_timestep: f64,         // Minimum allowed timestep to ensure that the solver finishes in a reasonable amount of time
        max_timestep: f64,         // Maximum allowed timestep to ensure a certain resolution of the solution
        steps_per_period: usize    // Target number of timesteps per current characteristic period
    }
}

#[derive(Copy, Clone)]
pub enum StopCondition {
    Time(f64),                    // Simulate up to a specific point in time
    Acceleration(Dof, f64, i32)   // Acceleration component to watch, critical value and relevant sign change
}

#[derive(PartialEq, Debug, Copy, Clone)]
pub enum DynamicSolverError {
    EquilibriumError(NewtonError),    // Error during the equilibrium iterations
    SingularMassMatrix,               // The mass matrix is singular, i.e. cannot be inverted
    MaxTimeReached,                   // Maximum time has been reached without stopping event
    AbortedByCaller                   // Aborted by the callback function's return value
}

impl Display for DynamicSolverError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            DynamicSolverError::EquilibriumError(error) => Display::fmt(&error, f)?,
            DynamicSolverError::SingularMassMatrix      => write!(f, "The system has a singular mass matrix.")?,
            DynamicSolverError::MaxTimeReached          => write!(f, "Maximum time has been reached without encountering a stopping condition.")?,
            DynamicSolverError::AbortedByCaller         => write!(f, "Aborted by the caller.")?,
        }

        Ok(())
    }
}

impl std::error::Error for DynamicSolverError {

}

pub struct DynamicSolver<'a> {
    system: &'a mut System,
    settings: DynamicSolverSettings
}

impl<'a> DynamicSolver<'a> {
    pub fn new(system: &'a mut System, settings: DynamicSolverSettings) -> Self {
        Self {
            system,
            settings
        }
    }

    pub fn solve<F>(&mut self, stop: StopCondition, callback: &mut F) -> Result<(), DynamicSolverError>
        where F: FnMut(&System, &SystemEval) -> bool
    {
        // Constant average acceleration (unconditionally stable for linear systems)
        let beta  = 0.25;
        let gamma = 0.50;

        // The mass matrix has to be evaluated only once
        let mut M = DVector::zeros(self.system.n_dofs());
        self.system.compute_mass_matrix(&mut M);

        // Dynamic system properties for evaluation
        let mut K_eval = DMatrix::zeros(self.system.n_dofs(), self.system.n_dofs());
        let mut D_eval = DMatrix::zeros(self.system.n_dofs(), self.system.n_dofs());
        let mut p_eval = DVector::zeros(self.system.n_dofs());
        let mut q_eval = DVector::zeros(self.system.n_dofs());
        let mut a_eval = DVector::zeros(self.system.n_dofs());

        // Check if the mass matrix is positive definite
        if M.amax() < 0.0 {
            return Err(DynamicSolverError::SingularMassMatrix);
        }

        // The time step is either set to a fixed value or, in the case of adaptive stepping, initially to the minimum threshold.
        let mut dt = match self.settings.time_stepping {
            TimeStepping::Fixed(dt) => dt,
            TimeStepping::Adaptive{ min_timestep, .. } => min_timestep
        };

        // Time at the start and end of a timestep
        let mut t_prev = self.system.get_time();
        let mut t_next = t_prev + dt;

        // If the stopping condition is an end time, verify that it is larger than the current system time
        // This would be a programming error and is therefore handled by an assert.
        if let StopCondition::Time(t_end) = stop {
            assert!(t_prev <= t_end, "Stopping time must not be smaller than the current system time");
        }

        // Evaluate system and invoke callback before the first timestep in order to provide information at t = t0.
        // After this we only invoke the callback at the end of the timesteps.
        self.system.compute_external_forces(&mut p_eval);
        self.system.compute_internal_forces(Some(&mut q_eval), Some(&mut K_eval), Some(&mut D_eval));
        a_eval.copy_from(&(&p_eval - &q_eval).component_div(&M));

        if !callback(self.system, &SystemEval::new(&p_eval, &q_eval, &a_eval)) {
            return Ok(());
        }

        loop {
            // Handle time-based termination here if the stopping condition is a time
            if let StopCondition::Time(t_stop) = stop {
                // If the end of the next time interval exceeds the stopping time,
                // truncate it to the stopping time and adjust the timestep accordingly
                if t_next > t_stop {
                    t_next = t_stop;
                    dt = t_next - t_prev;
                }
                // If the start of the next time interval is equal to the stopping time (might have been truncated in the last step),
                // end the simulation here and don't perform that next time step.
                if t_prev == t_stop {
                    return Ok(());
                }
            }
            else {
                // If the stopping condition is not time-based, check here for timeout
                if t_prev > self.settings.max_time {
                    return Err(DynamicSolverError::MaxTimeReached);
                }
            }

            // Current displacements, velocities and accelerations
            let u_prev = self.system.get_displacements().clone();
            let v_prev = self.system.get_velocities().clone();
            let a_prev = a_eval.clone();

            let mut u_next = DVector::<f64>::zeros(u_prev.len());
            let mut v_next = DVector::<f64>::zeros(u_prev.len());

            // Residuum function
            // Input: Accelerations at next time point
            // Output: Residuum and its jacobian at next time point
            let mut residuum = |a_next: &DVector<f64>, r_next: &mut DVector<f64>, drda: &mut DMatrix<f64>| {
                v_next.copy_from(&(&v_prev + dt*(1.0 - gamma)*&a_prev + dt*gamma*a_next));
                u_next.copy_from(&(&u_prev + dt*&v_prev + dt*dt*((0.5 - beta)*&a_prev + beta*a_next)));

                self.system.set_time(t_next);
                self.system.set_displacements(&u_next);
                self.system.set_velocities(&v_next);

                self.system.compute_external_forces(&mut p_eval);
                self.system.compute_internal_forces(Some(&mut q_eval), Some(&mut K_eval), Some(&mut D_eval));
                a_eval.copy_from(&(&p_eval - &q_eval).component_div(&M));

                r_next.copy_from(&(M.component_mul(a_next) + &q_eval - &p_eval));
                drda.copy_from(&(DMatrix::<f64>::from_diagonal(&M) + dt*gamma*&D_eval + dt*dt*beta*&K_eval));
            };

            solve_newton(&mut residuum, a_prev.clone(), self.settings.newton)
                .map_err(DynamicSolverError::EquilibriumError)?;

            // If the termination is based on acceleration, check here for sign changes
            if let StopCondition::Acceleration(dof, value, sign) = stop {
                let index = match dof {
                    Dof::Free(i) => i,
                    Dof::Fixed(_) => panic!("Can't watch acceleration of a fixed dof for sign changes")
                };

                let positive = (a_prev[index] < value) && (a_eval[index] > value);    // Positive sign change occurred
                let negative = (a_prev[index] > value) && (a_eval[index] < value);    // Negative sign change occurred

                // If a sign change occurred that matches the specified option, the end of the simulation has been reached and exceeded.
                // To get an accurate end time, the equilibrium iteration is repeated with the timestep as a free parameter
                // and the additional constraint that the acceleration must reach the given value at the end of the interval.
                if (sign >= 0 && positive) || (sign <= 0 && negative) {
                    // Residuum function as above, but with dt as an additional scalar parameter
                    let mut residuum = |a_next: &DVector<f64>, dt: f64, r_next: &mut DVector<f64>, drda: &mut DMatrix<f64>, drddt: &mut DVector<f64>| {
                        v_next.copy_from(&(&v_prev + dt*(1.0 - gamma)*&a_prev + dt*gamma*a_next));
                        u_next.copy_from(&(&u_prev + dt*&v_prev + dt*dt*((0.5 - beta)*&a_prev + beta*a_next)));

                        self.system.set_time(t_prev + dt);
                        self.system.set_displacements(&u_next);
                        self.system.set_velocities(&v_next);

                        self.system.compute_external_forces(&mut p_eval);
                        self.system.compute_internal_forces(Some(&mut q_eval), Some(&mut K_eval), Some(&mut D_eval));
                        a_eval.copy_from(&(&p_eval - &q_eval).component_div(&M));

                        r_next.copy_from(&(&M.component_mul(a_next) + &q_eval - &p_eval));
                        drda.copy_from(&(DMatrix::<f64>::from_diagonal(&M) + dt*gamma*&D_eval + dt*dt*beta*&K_eval));
                        drddt.copy_from(&(&K_eval*(&v_prev + 2.0*dt*((0.5 - beta)*&a_prev + beta*a_next)) + &D_eval*((1.0 - gamma)*&a_prev + gamma*a_next)));
                    };

                    // Constraint for the acceleration
                    let mut constraint = |a_next: &DVector<f64>, _dt: f64, c: &mut f64, dcda: &mut DVector<f64>, dcddt: &mut f64| {
                        *c = a_next[index] - value;
                        *dcddt = 0.0;

                        dcda.fill(0.0);
                        dcda[index] = 1.0;
                    };

                    // Solve the constrained problem
                    solve_newton_constrained(&mut residuum, &mut constraint, a_prev.clone(), dt, self.settings.newton)
                        .map_err(DynamicSolverError::EquilibriumError)?;

                    // Invoke callback with final system state, then end the simulation
                    if !callback(self.system, &SystemEval::new(&p_eval, &q_eval, &a_eval)) {
                        return Err(DynamicSolverError::AbortedByCaller);
                    }

                    return Ok(());
                }
            }

            // Invoke callback with final system state
            if !callback(self.system, &SystemEval::new(&p_eval, &q_eval, &a_eval)) {
                return Err(DynamicSolverError::AbortedByCaller);
            }

            // Determine the next timestep in case of adaptive time stepping
            if let TimeStepping::Adaptive { min_timestep, max_timestep, steps_per_period, .. } = self.settings.time_stepping {
                // Displacement increment during the last time step
                let delta_u = &u_next - &u_prev;

                // Current characteristic frequency
                let num: f64 = delta_u.dot(&(&K_eval*&delta_u));
                let den: f64 = delta_u.dot(&(M.component_mul(&delta_u)));
                let omega = f64::sqrt(f64::abs(num/den));

                let dt_suggested = std::f64::consts::TAU/(omega*(steps_per_period as f64));    // Timestep suggested by the number of steps per characteristic period
                let dt_suggested = f64::min(dt_suggested/dt, 2.0)*dt;                          // Limit relative increase with respect to the previous timestep
                dt = num::clamp(dt_suggested, min_timestep, max_timestep);                     // Limit absolute value to minimum and maximum settings
            }

            // Shift times to the upcoming time interval
            t_prev = t_next;
            t_next += dt;
        }
    }


    /*
    pub fn solve<F>(&mut self, stop: StopCondition, callback: &mut F) -> Result<(), DynamicSolverError>
        where F: FnMut(&System, &SystemEval) -> bool
    {
        // Constant average acceleration (unconditionally stable for linear systems)
        let beta  = 0.25;
        let gamma = 0.50;

        // Compute the constant mass matrix
        let mut M = DVector::zeros(self.system.n_dofs());
        self.system.compute_mass_matrix(&mut M);

        // Dynamic evaluation results
        let mut p = DVector::zeros(self.system.n_dofs());
        let mut q = DVector::zeros(self.system.n_dofs());
        let mut K = DMatrix::zeros(self.system.n_dofs(), self.system.n_dofs());
        let mut D = DMatrix::zeros(self.system.n_dofs(), self.system.n_dofs());

        // Check if the mass matrix is positive definite
        if M.amax() < 0.0 {
            return Err(DynamicSolverError::SingularMassMatrix);
        }

        // The time step is either set to a fixed value or, in the case of adaptive stepping, initially to the minimum threshold.
        let mut dt = match self.settings.time_stepping {
            TimeStepping::Fixed(dt) => dt,
            TimeStepping::Adaptive{ min_timestep, .. } => min_timestep
        };

        // Time at the start and end of a timestep
        let mut t_prev = self.system.get_time();
        let mut t_next = t_prev + dt;

        // If the stopping condition is an end time, verify that it is larger than the current system time
        // This would be a programming error and is therefore handled by an assert.
        if let StopCondition::Time(t_end) = stop {
            assert!(t_prev <= t_end, "Stopping time must not be smaller than the current system time");
        }

        // Evaluate system and invoke callback before the first timestep in order to provide information at t = t0.
        // After this we only invoke the callback at the end of the timesteps.
        self.system.compute_external_forces(&mut p);
        self.system.compute_internal_forces(Some(&mut q), Some(&mut K), Some(&mut D));
        let a = (&p - &q).component_div(&M);
        if !callback(self.system, &SystemEval::new(&p, &q, &a)) {
            return Ok(());
        }

        loop {
            // Handle time-based termination here if the stopping condition is a time
            if let StopCondition::Time(t_stop) = stop {
                // If the end of the next time interval exceeds the stopping time,
                // truncate it to the stopping time and adjust the timestep accordingly
                if t_next > t_stop {
                    t_next = t_stop;
                    dt = t_next - t_prev;
                }
                // If the start of the next time interval is equal to the stopping time (might have been truncated in the last step),
                // end the simulation here and don't perform that next time step.
                if t_prev == t_stop {
                    return Ok(());
                }
            }
            else {
                // If the stopping condition is not time-based, check here for timeout
                if t_prev > self.settings.max_time {
                    return Err(DynamicSolverError::MaxTimeReached);
                }
            }

            // Current displacements, velocities and accelerations
            let u_prev = self.system.get_displacements().clone();
            let v_prev = self.system.get_velocities().clone();
            let a_prev = a.clone();

            let mut u_next = DVector::<f64>::zeros(u_prev.len());
            let mut v_next = DVector::<f64>::zeros(u_prev.len());

            // Residuum function
            // Input: Accelerations at next time point
            // Output: Residuum and its jacobian at next time point
            let mut residuum = |a_next: &DVector<f64>, r_next: &mut DVector<f64>, drda: &mut DMatrix<f64>| {
                v_next.copy_from(&(&v_prev + dt*(1.0 - gamma)*&a_prev + dt*gamma*a_next));
                u_next.copy_from(&(&u_prev + dt*&v_prev + dt*dt*((0.5 - beta)*&a_prev + beta*a_next)));

                self.system.set_time(t_next);
                self.system.set_displacements(&u_next);
                self.system.set_velocities(&v_next);

                self.system.compute_external_forces(&mut p);
                self.system.compute_internal_forces(Some(&mut q), Some(&mut K), Some(&mut D));

                r_next.copy_from(&(M.component_mul(a_next) + &q - &p));
                drda.copy_from(&(DMatrix::<f64>::from_diagonal(&M) + dt*gamma*&D + dt*dt*beta*&K));
            };

            let result = solve_newton(&mut residuum, a_prev.clone(), self.settings.newton).map_err(DynamicSolverError::EquilibriumError)?;
            let a_next = result.x;

            // If the termination is based on acceleration, check here for sign changes
            if let StopCondition::Acceleration(dof, value, sign) = stop {
                let index = match dof {
                    Dof::Free(i) => i,
                    Dof::Fixed(_) => panic!("Can't watch acceleration of a fixed dof for sign changes")
                };

                let positive = (a_prev[index] < value) && (a_next[index] > value);    // Positive sign change occurred
                let negative = (a_prev[index] > value) && (a_next[index] < value);    // Negative sign change occurred

                // If a sign change occurred that matches the specified option, the end of the simulation has been reached and exceeded.
                // To get an accurate end time, the equilibrium iteration is repeated with the timestep as a free parameter
                // and the additional constraint that the acceleration must reach the given value at the end of the interval.
                if (sign >= 0 && positive) || (sign <= 0 && negative) {
                    // Residuum function as above, but with dt as an additional scalar parameter
                    let mut residuum = |a_next: &DVector<f64>, dt: f64, r_next: &mut DVector<f64>, drda: &mut DMatrix<f64>, drddt: &mut DVector<f64>| {
                        v_next.copy_from(&(&v_prev + dt*(1.0 - gamma)*&a_prev + dt*gamma*a_next));
                        u_next.copy_from(&(&u_prev + dt*&v_prev + dt*dt*((0.5 - beta)*&a_prev + beta*a_next)));

                        self.system.set_time(t_prev + dt);
                        self.system.set_displacements(&u_next);
                        self.system.set_velocities(&v_next);

                        self.system.compute_external_forces(&mut p);
                        self.system.compute_internal_forces(Some(&mut q), Some(&mut K), Some(&mut D));

                        r_next.copy_from(&(M.component_mul(a_next) + &q - &p));
                        drda.copy_from(&(DMatrix::<f64>::from_diagonal(&M) + dt*gamma*&D + dt*dt*beta*&K));
                        drddt.copy_from(&(&K*(&v_prev + 2.0*dt*((0.5 - beta)*&a_prev + beta*a_next)) + &D*((1.0 - gamma)*&a_prev + gamma*a_next)));
                    };

                    // Constraint for the acceleration
                    let mut constraint = |a_next: &DVector<f64>, _dt: f64, c: &mut f64, dcda: &mut DVector<f64>, dcddt: &mut f64| {
                        *c = a_next[index] - value;
                        *dcddt = 0.0;

                        dcda.fill(0.0);
                        dcda[index] = 1.0;
                    };

                    // Solve the constrained problem
                    let result = solve_newton_constrained(&mut residuum, &mut constraint, a_prev.clone(), dt, self.settings.newton).map_err(DynamicSolverError::EquilibriumError)?;
                    let a_next = result.x;

                    // Invoke callback with final system state, then end the simulation
                    if !callback(self.system, &SystemEval::new(&p, &q, &a_next)) {
                        return Err(DynamicSolverError::AbortedByCaller);
                    }

                    return Ok(());
                }
            }

            // Invoke callback with final system state
            if !callback(self.system, &SystemEval::new(&p, &q, &a_next)) {
                return Err(DynamicSolverError::AbortedByCaller);
            }

            // Determine the next timestep in case of adaptive time stepping
            if let TimeStepping::Adaptive { min_timestep, max_timestep, steps_per_period, .. } = self.settings.time_stepping {
                // Displacement increment during the last time step
                let delta_u = &u_next - &u_prev;

                // Current characteristic frequency
                let num: f64 = delta_u.dot(&(&K*&delta_u));
                let den: f64 = delta_u.dot(&(M.component_mul(&delta_u)));
                let omega = f64::sqrt(f64::abs(num/den));

                let dt_suggested = std::f64::consts::TAU/(omega*(steps_per_period as f64));    // Timestep suggested by the number of steps per characteristic period
                let dt_suggested = f64::min(dt_suggested/dt, 2.0)*dt;                          // Limit relative increase with respect to the previous timestep
                dt = num::clamp(dt_suggested, min_timestep, max_timestep);                     // Limit absolute value to minimum and maximum settings
            }

            // Shift times to the upcoming time interval
            t_prev = t_next;
            t_next += dt;
        }
    }
    */
}