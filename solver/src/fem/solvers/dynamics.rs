use nalgebra::DVector;
use crate::fem::system::system::{System, DynamicEval};

#[derive(Copy, Clone)]
pub struct Settings {
    pub timestep: f64,       // Relative tolerance
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            timestep: 1e-6
        }
    }
}

pub struct DynamicSolverRK4<'a> {
    system: &'a mut System,
    settings: Settings
}

impl<'a> DynamicSolverRK4<'a> {
    pub fn new(system: &'a mut System, settings: Settings) -> Self {
        Self {
            system,
            settings
        }
    }

    pub fn solve<F>(&mut self, callback: &mut F)
        where F: FnMut(&System, &DynamicEval) -> bool
    {
        let n = self.system.n_dofs();
        let dt = self.settings.timestep;

        // Dynamic system state
        let mut dynamics = self.system.default_dynamic_eval();

        // Pre-allocated variables
        let mut t = 0.0;
        let mut x = DVector::<f64>::zeros(2*n);
        let mut k1 = DVector::<f64>::zeros(2*n);
        let mut k2 = DVector::<f64>::zeros(2*n);
        let mut k3 = DVector::<f64>::zeros(2*n);
        let mut k4 = DVector::<f64>::zeros(2*n);

        // Set initial state
        x.rows_mut(0, n).copy_from(&self.system.get_displacements());
        x.rows_mut(n, n).copy_from(&self.system.get_velocities());

        // TODO: Prevent duplicate evaluations
        self.system.eval_dynamics(&mut dynamics);
        if !callback(&self.system, &dynamics) {
            return;
        }

        loop {
            let mut function = |t: f64, x: &DVector::<f64>, dxdt: &mut DVector<f64>| {
                self.system.set_displacements(&x.rows(0, n).into());
                self.system.set_velocities(&x.rows(n, n).into());
                self.system.set_time(t);
                self.system.eval_dynamics(&mut dynamics);

                dxdt.rows_mut(0, n).copy_from(&self.system.get_velocities());
                dxdt.rows_mut(n, n).copy_from(&dynamics.get_accelerations());
            };

            function(t         ,                        &x, &mut k1);
            function(t + 0.5*dt, &(&x + &k1.scale(0.5*dt)), &mut k2);
            function(t + 0.5*dt, &(&x + &k2.scale(0.5*dt)), &mut k3);
            function(t + dt    , &(&x + &k3.scale(0.5*dt)), &mut k4);

            x += &(&k1 + &k2.scale(2.0) + &k3.scale(2.0) + &k4).scale(dt/6.0);
            t += dt;

            self.system.set_time(t);
            self.system.set_displacements(&x.rows(0, n).into());
            self.system.set_velocities(&x.rows(n, n).into());
            // TODO: Prevent duplicate evaluations
            self.system.eval_dynamics(&mut dynamics);

            if !callback(&self.system, &dynamics) {
                break;
            }
        }
    }
}