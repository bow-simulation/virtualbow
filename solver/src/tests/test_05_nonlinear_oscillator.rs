use std::f64::consts::PI;
use spec_math::Ellip;
use crate::fem::elements::bar::BarElement;
use crate::fem::elements::mass::MassElement;
use crate::fem::solvers::dynamics::{DynamicSolver, MethodParameters, Settings};
use crate::fem::system::nodes::Constraints;
use crate::fem::system::system::System;
use crate::tests::utils;
use crate::tests::utils::plotter::Plotter;

#[test]
fn nonlinear_pendulum() {
    // In this example, a simple pendulum is simulated and the results are compared with an analytic solution according to [1].
    // This tests how the dynamic solver deals with a simple nonlinear system where the equilibrium iterations at each time step actually have to do something.
    // The pendulum bar is modelled as a spring, so the systems are not exactly equivalent.
    // [1] https://www.scielo.br/j/rbef/a/ns9Lc7tfqhZh678dBPXxRsQ/?format=pdf&lang=en

    // Pendulum parameters
    let g = 9.81;
    let l = 1.5;
    let m = 0.5;
    let k = 1e5;
    let d = 0.0;

    // Initial position
    let φ0 = 0.99*PI;
    let x0 =  (l - m*g/k)*f64::sin(φ0);
    let y0 = -(l - m*g/k)*f64::cos(φ0);

    let omega0 = f64::sqrt(g/l);
    let c = f64::sin(φ0 /2.0);

    let mut system = System::new();
    let node_a = system.create_xy_node(0.0, 0.0, Constraints::all_fixed());
    let node_b = system.create_xy_node(x0, y0, Constraints::all_free());

    system.add_element(&[node_a, node_b], BarElement::spring(0.0, d, k, l));
    system.add_element(&[node_b], MassElement::new(m));
    system.add_force(node_b.y(), move |_t| -m*g);

    utils::checks::check_system_invariants(&mut system);

    let mut plotter = Plotter::new();
    let mut solver = DynamicSolver::new(&mut system, Settings { timestep: 1e-3, method: MethodParameters::newmark(), ..Default::default() });

    solver.solve(&mut |system, eval| {
        let t = system.get_time();

        // Analytical solution
        let ellip = f64::ellip_j(&(f64::ellip_k(&c.powi(2)) - omega0*t), c.powi(2));
        let φ_ref = 2.0*f64::asin(c*ellip.sn);
        let φ_dot_ref = -2.0*c*omega0/f64::sqrt(1.0 - (c*ellip.sn).powi(2))*ellip.cn*ellip.dn;
        let φ_ddot_ref = -omega0*omega0*f64::sin(φ_ref);

        let x_ref = l*f64::sin(φ_ref);
        let x_dot_ref = l*φ_dot_ref*f64::cos(φ_ref);
        let x_ddot_ref = l*φ_ddot_ref*f64::cos(φ_ref) - l*φ_dot_ref*φ_dot_ref*f64::sin(φ_ref);

        let y_ref = -l*f64::cos(φ_ref);
        let y_dot_ref = l*φ_dot_ref*f64::sin(φ_ref);
        let y_ddot_ref = l*φ_ddot_ref*f64::sin(φ_ref) + l*φ_dot_ref*φ_dot_ref*f64::cos(φ_ref);

        // Numerical solution
        let x_num = system.get_displacement(node_b.x());
        let x_dot_num = system.get_velocity(node_b.x());
        let x_ddot_num = eval.get_acceleration(node_b.x());

        let y_num = system.get_displacement(node_b.y());
        let y_dot_num = system.get_velocity(node_b.y());
        let y_ddot_num = eval.get_acceleration(node_b.y());

        plotter.add_point((t, 0.0), (t, φ_ref), "position φ", "Time [s]", "φ [rad]");
        plotter.add_point((t, 0.0), (t, φ_dot_ref), "velocity φ", "Time [s]", "d/dt φ [rad/s]");
        plotter.add_point((t, 0.0), (t, φ_ddot_ref), "acceleration φ", "Time [s]", "d²/dt² φ [rad/s²]");

        plotter.add_point((t, x_num), (t, x_ref), "position x", "Time [s]", "x [m]");
        plotter.add_point((t, x_dot_num), (t, x_dot_ref), "velocity x", "Time [s]", "d/dt x [m]");
        plotter.add_point((t, x_ddot_num), (t, x_ddot_ref), "acceleration x", "Time [s]", "d²/dt² x [m]");

        plotter.add_point((t, y_num), (t, y_ref), "position y", "Time [s]", "y [m]");
        plotter.add_point((t, y_dot_num), (t, y_dot_ref), "velocity y", "Time [s]", "d/dt y [m]");
        plotter.add_point((t, y_ddot_num), (t, y_ddot_ref), "acceleration y", "Time [s]", "d²/dt² y [m]");

        assert_abs_diff_eq!(x_num, x_ref, epsilon=1e-2);
        assert_abs_diff_eq!(x_dot_num, x_dot_ref, epsilon=1e-2);
        assert_abs_diff_eq!(x_ddot_num, x_ddot_ref, epsilon=1e-1);

        assert_abs_diff_eq!(y_num, y_ref, epsilon=1e-2);
        assert_abs_diff_eq!(y_dot_num, y_dot_ref, epsilon=1e-2);
        assert_abs_diff_eq!(y_ddot_num, y_ddot_ref, epsilon=1e-1);

        return t < 10.0;
    });
}