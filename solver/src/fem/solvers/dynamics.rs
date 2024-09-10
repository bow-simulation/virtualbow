use nalgebra::{DMatrix, DVector};
use crate::fem::system::system::{System, DynamicEval};

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

    pub fn solve<F>(&mut self, callback: &mut F)
        where F: FnMut(&System, &DynamicEval) -> bool
    {
        // Constant average acceleration (unconditionally stable for linear systems)
        let beta  = 0.25;
        let gamma = 0.50;

        // Dynamic system state
        let mut eval = self.system.create_dynamic_eval();

        // Solver state
        let mut t = self.system.get_time();

        // First evaluation at start of the simulated interval to make acceleration available
        // and provide callback information at t = t0.
        self.system.eval_dynamics(&mut eval);
        if !callback(&self.system, &eval) {
            return;
        }

        let dt = self.settings.timestep;

        loop {
            // Current displacements, velocities and accelerations
            let u_current = self.system.get_displacements().clone();
            let v_current = self.system.get_velocities().clone();
            let a_current = eval.get_accelerations().clone();

            let mut a_next = a_current.clone();  // DVector::zeros(n);
            let mut v_next = &v_current + dt*(1.0 - gamma)*&a_current + dt*gamma*&a_next;
            let mut u_next = &u_current + dt*&v_current + dt*dt*((0.5 - beta)*&a_current + beta*&a_next);

            // Next acceleration to iterate on
            for i in 0..self.settings.max_iterations {
                self.system.set_time(t + dt);
                self.system.set_displacements(&u_next);
                self.system.set_velocities(&v_next);
                self.system.eval_dynamics(&mut eval);

                let r: DVector<f64> = eval.get_external_forces() - eval.get_internal_forces() - eval.get_mass_matrix().component_mul(&a_next);
                let drda: DMatrix<f64> = DMatrix::<f64>::from_diagonal(eval.get_mass_matrix()) + dt*gamma*eval.get_damping_matrix() + dt*dt*beta*eval.get_stiffness_matrix();

                let decomposition = drda.lu();
                let delta_a = decomposition.solve(&r).unwrap();
                if delta_a.amax() < self.settings.epsilon_rel*a_next.amax() + self.settings.epsilon_abs {
                    break;
                }

                a_next += &delta_a;
                v_next += dt*gamma*&delta_a;
                u_next += dt*dt*beta*&delta_a;
            }

            t += dt;

            if !callback(&self.system, &eval) {
                break;
            }
        }
    }
}