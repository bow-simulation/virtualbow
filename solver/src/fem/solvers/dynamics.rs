use nalgebra::{DMatrix, DVector};
use crate::fem::system::system::{System, DynamicEval};

#[derive(Copy, Clone)]
pub struct Settings {
    pub method: MethodParameters,
    pub timestep: f64,          // Time step
    pub epsilon_rel: f64,       // Relative tolerance
    pub epsilon_abs: f64,       // Absolute tolerance
    pub max_iterations: u32,    // Maximum number of iterations per load step
    pub max_stagnation: u32     // Maximum number of iterations that don't improve the objective
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            method: MethodParameters::newmark(),
            timestep: 1e-6,
            epsilon_rel: 1e-08,
            epsilon_abs: 1e-10,
            max_iterations: 50,
            max_stagnation: 10
        }
    }
}

#[derive(Copy, Clone)]
pub struct MethodParameters {
    alpha_m: f64,
    alpha_f: f64,
    beta: f64,
    gamma: f64,
}

impl MethodParameters {
    // Newmark with parameters for average constant acceleration (unconditionally stable in the linear case)
    pub fn newmark() -> Self {
        Self {
            alpha_m: 0.0,
            alpha_f: 0.0,
            beta: 0.25,
            gamma: 0.50,
        }
    }

    // Generalized alpha method with spectral radium rho_inf € [0, 1] at infinity
    pub fn gen_alpha(rho_inf: f64) -> Self {
        assert!(rho_inf >= 0.0 && rho_inf <= 1.0);

        let alpha_m = (2.0*rho_inf - 1.0)/(rho_inf + 1.0);
        let alpha_f = rho_inf/(rho_inf + 1.0);

        Self {
            beta: 0.25*(1.0 - alpha_m + alpha_f).powi(2),
            gamma: 0.5 - alpha_m + alpha_f,
            alpha_m,
            alpha_f,
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
        // Commonly used settings
        let alpha_m = self.settings.method.alpha_m;
        let alpha_f = self.settings.method.alpha_f;
        let beta  = self.settings.method.beta;
        let gamma = self.settings.method.gamma;
        let dt = self.settings.timestep;

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

        loop {
            // Current displacements, velocities and accelerations
            let u_current = self.system.get_displacements().clone();
            let v_current = self.system.get_velocities().clone();
            let a_current = eval.get_accelerations().clone();
            let p_current = eval.get_external_forces().clone();
            let q_current = eval.get_internal_forces().clone();

            // Next displacements, velocities and accelerations to be iterated on
            let mut a_next = a_current.clone();  // Initial value is last value
            let mut v_next = &v_current + dt*(1.0 - gamma)*&a_current + dt*gamma*&a_next;
            let mut u_next = &u_current + dt*&v_current + dt*dt*((0.5 - beta)*&a_current + beta*&a_next);

            // Next acceleration to iterate on
            for i in 0..self.settings.max_iterations {
                self.system.set_time(t + dt);
                self.system.set_displacements(&u_next);
                self.system.set_velocities(&v_next);
                self.system.eval_dynamics(&mut eval);

                let p_next = eval.get_external_forces();
                let q_next = eval.get_internal_forces();

                let a_alpha = (1.0 - alpha_m)*&a_next + alpha_m*&a_current;
                let p_alpha = (1.0 - alpha_f)*p_next + alpha_f*&p_current;
                let q_alpha = (1.0 - alpha_f)*q_next + alpha_f*&q_current;

                let r: DVector<f64> = p_alpha - q_alpha - eval.get_mass_matrix().component_mul(&a_alpha);
                let drda: DMatrix<f64> = (1.0 - alpha_m)*DMatrix::<f64>::from_diagonal(eval.get_mass_matrix()) + (1.0 - alpha_f)*(dt*gamma*eval.get_damping_matrix() + dt*dt*beta*eval.get_stiffness_matrix());

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