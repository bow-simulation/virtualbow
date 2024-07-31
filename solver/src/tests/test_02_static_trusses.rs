use std::f64::consts::SQRT_2;
use crate::fem::elements::bar::BarElement;
use crate::fem::solvers::statics::{Settings, StaticSolver};
use crate::fem::system::nodes::Constraints;
use crate::fem::system::system::System;
use crate::tests::utils;
use crate::tests::utils::plotter::Plotter;

// These tests solve various linear and nonlinear static bar trusses
// and compare the results with analytical reference solutions.
// See the theoretical documentation for the examples and their solutions.

#[test]
fn linear_bar_truss_1() {
    let a = 1.0;
    let EA = 21000.0;
    let rhoA = 0.785;

    let x_ref = a + 0.001;           // Reference displacement
    let F_ref = EA/a*(x_ref - a);    // Reference force

    let mut system = System::new();
    let node1 = system.create_xy_node(0.0, 0.0, Constraints::all_fixed());
    let node2 = system.create_xy_node(a, 0.0, Constraints::x_pos_free());

    system.add_element(&[node1, node2], BarElement::new(rhoA, 0.0, EA, a));
    system.add_force(node2.x(), move |_t|{ F_ref });

    utils::checks::check_system_invariants(&mut system);

    let mut solver = StaticSolver::new(&mut system, Settings::default());
    solver.solve_equilibrium_load_controlled(1.0).expect("Static solver failed");

    let x_sys = system.get_displacement(node2.x());
    assert_relative_eq!(x_sys, x_ref, max_relative=1e-6);
}

#[test]
fn linear_bar_truss_2() {
    // TODO: Test for linear constrained dofs
}

#[test]
fn linear_bar_truss_3() {
    let a = 1.0;
    let EA = 21000.0;
    let rhoA = 0.785;

    let F_ref = 10.0;
    let x_ref = a - 1.0/4.0*F_ref*a/EA;
    let y_ref = a - 3.0/4.0*F_ref*a/EA;

    let mut system = System::new();

    let node1 = system.create_xy_node(0.0,   0.0, Constraints::all_fixed());
    let node2 = system.create_xy_node(  a,   0.0, Constraints::all_free());
    let node3 = system.create_xy_node(  a,     a, Constraints::all_free());
    let node4 = system.create_xy_node(0.0, 2.0*a, Constraints::all_fixed());

    system.add_element(&[node1, node2], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node2, node3], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node1, node3], BarElement::new(rhoA, 0.0, EA, SQRT_2*a));
    system.add_element(&[node3, node4], BarElement::new(rhoA, 0.0, 2.0*EA, SQRT_2*a));

    system.add_force(node2.x(), move |_t|{ F_ref/SQRT_2 });
    system.add_force(node2.y(), move |_t|{ -F_ref/SQRT_2 });

    utils::checks::check_system_invariants(&mut system);

    let mut solver = StaticSolver::new(&mut system, Settings::default());
    solver.solve_equilibrium_load_controlled(1.0).expect("Static solver failed");

    assert_relative_eq!(system.get_displacement(node3.x()), x_ref, max_relative=1e-3);
    assert_relative_eq!(system.get_displacement(node3.y()), y_ref, max_relative=1e-3);
}

#[test]
fn linear_bar_truss_4() {
    let a = 0.5;
    let EA = 21000.0;
    let rhoA = 0.785;

    let F_ref = 10.0;
    let s_ref = (4.0 + 2.0*SQRT_2)*F_ref* a /EA;

    let mut system = System::new();
    let node_01 = system.create_xy_node(  0.0, 0.0, Constraints::pos_fixed());
    let node_02 = system.create_xy_node(    a, 0.0, Constraints::all_free());
    let node_03 = system.create_xy_node(2.0*a, 0.0, Constraints::all_free());
    let node_04 = system.create_xy_node(3.0*a, 0.0, Constraints::all_free());
    let node_05 = system.create_xy_node(4.0*a, 0.0, Constraints::x_pos_free());
    let node_06 = system.create_xy_node(  0.0,   a, Constraints::all_free());
    let node_07 = system.create_xy_node(    a,   a, Constraints::all_free());
    let node_08 = system.create_xy_node(2.0*a,   a, Constraints::all_free());
    let node_09 = system.create_xy_node(3.0*a,   a, Constraints::all_free());
    let node_10 = system.create_xy_node(4.0*a,   a, Constraints::all_free());

    system.add_element(&[node_01, node_02], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_02, node_03], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_03, node_04], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_04, node_05], BarElement::new(rhoA, 0.0, EA, a));

    system.add_element(&[node_06, node_07], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_07, node_08], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_08, node_09], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_09, node_10], BarElement::new(rhoA, 0.0, EA, a));

    system.add_element(&[node_01, node_06], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_02, node_07], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_03, node_08], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_04, node_09], BarElement::new(rhoA, 0.0, EA, a));
    system.add_element(&[node_05, node_10], BarElement::new(rhoA, 0.0, EA, a));

    system.add_element(&[node_01, node_07], BarElement::new(rhoA, 0.0, EA, SQRT_2* a));
    system.add_element(&[node_07, node_03], BarElement::new(rhoA, 0.0, EA, SQRT_2* a));
    system.add_element(&[node_03, node_09], BarElement::new(rhoA, 0.0, EA, SQRT_2* a));
    system.add_element(&[node_09, node_05], BarElement::new(rhoA, 0.0, EA, SQRT_2* a));

    system.add_force(node_02.y(), move |_t|{ -F_ref });
    system.add_force(node_04.y(), move |_t|{ -F_ref });

    utils::checks::check_system_invariants(&mut system);

    let mut solver = StaticSolver::new(&mut system, Settings::default());
    solver.solve_equilibrium_load_controlled(1.0).expect("Static solver failed");

    assert_relative_eq!(system.get_displacement(node_03.x()), 2.0*a, max_relative=1e-3);
    assert_relative_eq!(system.get_displacement(node_03.y()), -s_ref, max_relative=1e-3);
}

#[test]
fn nonlinear_bar_truss_1() {
    let a = 1.0;
    let b = 0.1;
    let EA = 21000.0;
    let rhoA = 0.785;

    let mut system = System::new();
    let n1 = system.create_xy_node(0.0, 0.0, Constraints::all_fixed());
    let n2 = system.create_xy_node(a, b, Constraints::y_pos_free());
    let e1 = system.add_element(&[n1, n2], BarElement::new(rhoA, 0.0, EA, f64::hypot(a, b)));
    system.add_force(n2.y(), |_t| { -1.0 });

    let mut plotter = Plotter::new();
    let mut solver = StaticSolver::new(&mut system, Settings::default());

    let result = solver.equilibrium_path_displacement_controlled(n2.y(), -b, 100, &mut |system, statics| {
        let y = system.get_displacement(n2.y());
        let ly = f64::hypot(a, y);
        let l0 = f64::hypot(a, b);

        let N_ref = EA*(ly - l0)/l0;
        let F_ref = y*N_ref/ly;

        let N_sys = system.element_ref::<BarElement>(e1).normal_force();
        let F_sys = statics.get_scaled_external_force(n2.y());

        plotter.add_point((y, F_sys), (y, F_ref), "Applied Force", "y [m]", "F [N]");
        plotter.add_point((y, N_sys), (y, N_ref), "Normal Force", "y [m]", "N [N]");

        assert_relative_eq!(N_sys, N_ref, max_relative=1e-6);
        assert_relative_eq!(F_sys, F_ref, max_relative=1e-6);

        true
    });

    assert!(result.is_ok());
}

#[test]
fn nonlinear_bar_truss_2() {
    let a = 0.4;
    let b = 0.6;
    let c = 0.1;
    let EA = 21000.0;
    let rhoA = 0.785;

    let mut system = System::new();
    let node0 = system.create_xy_node(  0.0, 0.0, Constraints::all_fixed());
    let node1 = system.create_xy_node(    a,   c, Constraints::all_free());
    let node2 = system.create_xy_node(a + b, 0.0, Constraints::all_fixed());

    let bar01 = system.add_element(&[node0, node1], BarElement::new(rhoA, 0.0, EA, f64::hypot(a, c)));
    let bar12 = system.add_element(&[node1, node2], BarElement::new(rhoA, 0.0, EA, f64::hypot(b, c)));

    system.add_force(node1.y(), |_t| { -1.0 });

    let mut plotter = Plotter::new();
    let mut solver = StaticSolver::new(&mut system, Settings::default());

    let result = solver.equilibrium_path_displacement_controlled(node1.y(), -c, 100, &mut |system, statics| {
        let x = system.get_displacement(node1.x());
        let y = system.get_displacement(node1.y());
        let F = statics.get_scaled_external_force(node1.y());

        let alpha = f64::atan(y/x);
        let beta = f64::atan(y/(a + b - x));

        let l1 = f64::hypot(x, y);
        let l2 = f64::hypot(a + b - x, y);

        let N1_ref = EA*(l1 - f64::hypot(a, c))/f64::hypot(a, c);
        let N2_ref = EA*(l2 - f64::hypot(b, c))/f64::hypot(b, c);

        let N1_sys = system.element_ref::<BarElement>(bar01).normal_force();
        let N2_sys = system.element_ref::<BarElement>(bar12).normal_force();

        plotter.add_point((y, N1_sys), (y, N1_ref), "Normal Force 1", "y [m]", "N [N]");
        plotter.add_point((y, N2_sys), (y, N2_ref), "Normal Force 2", "y [m]", "N [N]");

        // Check if normal forces in the bars match
        assert_abs_diff_eq!(N1_sys, N1_ref, epsilon=1e-6);
        assert_abs_diff_eq!(N2_sys, N2_ref, epsilon=1e-6);

        // Check if the equilibrium conditions are satisfied
        assert_abs_diff_eq!(-N1_ref*f64::cos(alpha) + N2_ref*f64::cos(beta), 0.0, epsilon=1e-6);
        assert_abs_diff_eq!(-N1_ref*f64::sin(alpha) - N2_ref*f64::sin(beta) + F, 0.0, epsilon=1e-6);

        true
    });

    assert!(result.is_ok());
}