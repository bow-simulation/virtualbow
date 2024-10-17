use std::f64::consts::SQRT_2;
use nalgebra::vector;
use crate::fem::elements::string::StringElement;
use crate::fem::solvers::statics::{Settings, StaticSolver};
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

    let x_ref = a + 0.001;           // Reference displacement
    let F_ref = EA/a*(x_ref - a);    // Reference force

    let mut system = System::new();
    let node1 = system.create_node(&vector![0.0, 0.0, 0.0], &[false, false, false]);
    let node2 = system.create_node(&vector![a, 0.0, 0.0], &[true, false, false]);

    system.add_element(&[node1, node2], StringElement::bar(EA, 0.0, a));
    system.add_force(node2.x(), move |_t|{ F_ref });

    utils::checks::check_system_invariants(&mut system);

    let mut solver = StaticSolver::new(&mut system, Settings::default());
    solver.equilibrium_load_controlled(1.0).unwrap();

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

    let F_ref = 10.0;
    let x_ref = a - 1.0/4.0*F_ref*a/EA;
    let y_ref = a - 3.0/4.0*F_ref*a/EA;

    let mut system = System::new();

    let node1 = system.create_node(&vector![0.0, 0.0, 0.0], &[false, false, false]);
    let node2 = system.create_node(&vector![a, 0.0, 0.0], &[true, true, false]);
    let node3 = system.create_node(&vector![a, a, 0.0], &[true, true, false]);
    let node4 = system.create_node(&vector![0.0, 2.0*a, 0.0], &[false, false, false]);

    system.add_element(&[node1, node2], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node2, node3], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node1, node3], StringElement::bar(EA, 0.0, SQRT_2*a));
    system.add_element(&[node3, node4], StringElement::bar(2.0*EA, 0.0, SQRT_2*a));

    system.add_force(node2.x(), move |_t|{ F_ref/SQRT_2 });
    system.add_force(node2.y(), move |_t|{ -F_ref/SQRT_2 });

    utils::checks::check_system_invariants(&mut system);

    let mut solver = StaticSolver::new(&mut system, Settings::default());
    solver.equilibrium_load_controlled(1.0).unwrap();

    assert_relative_eq!(system.get_displacement(node3.x()), x_ref, max_relative=1e-3);
    assert_relative_eq!(system.get_displacement(node3.y()), y_ref, max_relative=1e-3);
}

#[test]
fn linear_bar_truss_4() {
    let a = 0.5;
    let EA = 21000.0;

    let F_ref = 10.0;
    let s_ref = (4.0 + 2.0*SQRT_2)*F_ref*a/EA;

    let mut system = System::new();
    let node_01 = system.create_node(&vector![0.0, 0.0, 0.0], &[false, false, false]);
    let node_02 = system.create_node(&vector![a, 0.0, 0.0], &[true, true, false]);
    let node_03 = system.create_node(&vector![2.0*a, 0.0, 0.0], &[true, true, false]);
    let node_04 = system.create_node(&vector![3.0*a, 0.0, 0.0], &[true, true, false]);
    let node_05 = system.create_node(&vector![4.0*a, 0.0, 0.0], &[true, false, false]);
    let node_06 = system.create_node(&vector![0.0, a, 0.0], &[true, true, false]);
    let node_07 = system.create_node(&vector![a, a, 0.0], &[true, true, false]);
    let node_08 = system.create_node(&vector![2.0*a, a, 0.0], &[true, true, false]);
    let node_09 = system.create_node(&vector![3.0*a, a, 0.0], &[true, true, false]);
    let node_10 = system.create_node(&vector![4.0*a, a, 0.0], &[true, true, false]);

    system.add_element(&[node_01, node_02], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_02, node_03], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_03, node_04], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_04, node_05], StringElement::bar(EA, 0.0, a));

    system.add_element(&[node_06, node_07], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_07, node_08], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_08, node_09], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_09, node_10], StringElement::bar(EA, 0.0, a));

    system.add_element(&[node_01, node_06], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_02, node_07], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_03, node_08], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_04, node_09], StringElement::bar(EA, 0.0, a));
    system.add_element(&[node_05, node_10], StringElement::bar(EA, 0.0, a));

    system.add_element(&[node_01, node_07], StringElement::bar(EA, 0.0, SQRT_2*a));
    system.add_element(&[node_07, node_03], StringElement::bar(EA, 0.0, SQRT_2*a));
    system.add_element(&[node_03, node_09], StringElement::bar(EA, 0.0, SQRT_2*a));
    system.add_element(&[node_09, node_05], StringElement::bar(EA, 0.0, SQRT_2*a));

    system.add_force(node_02.y(), move |_t|{ -F_ref });
    system.add_force(node_04.y(), move |_t|{ -F_ref });

    utils::checks::check_system_invariants(&mut system);

    let mut solver = StaticSolver::new(&mut system, Settings::default());
    solver.equilibrium_load_controlled(1.0).unwrap();

    assert_relative_eq!(system.get_displacement(node_03.x()), 2.0*a, max_relative=1e-3);
    assert_relative_eq!(system.get_displacement(node_03.y()), -s_ref, max_relative=1e-3);
}

#[test]
fn nonlinear_bar_truss_1() {
    let a = 1.0;
    let b = 0.1;
    let EA = 21000.0;

    let mut system = System::new();
    let node1 = system.create_node(&vector![0.0, 0.0, 0.0], &[false, false, false]);
    let node2 = system.create_node(&vector![a, b, 0.0], &[false, true, false]);
    let element = system.add_element(&[node1, node2], StringElement::bar(EA, 0.0, f64::hypot(a, b)));
    system.add_force(node2.y(), |_t| { -1.0 });

    let mut plotter = Plotter::new();
    let mut solver = StaticSolver::new(&mut system, Settings::default());

    let result = solver.equilibrium_path_displacement_controlled(node2.y(), -b, 100, &mut |system, statics| {
        let y = system.get_displacement(node2.y());
        let ly = f64::hypot(a, y);
        let l0 = f64::hypot(a, b);

        let N_ref = EA*(ly - l0)/l0;
        let F_ref = y*N_ref/ly;

        let N_sys = system.element_ref::<StringElement>(element).normal_force();
        let F_sys = statics.get_scaled_external_force(node2.y());

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

    let mut system = System::new();
    let node0 = system.create_node(&vector![0.0, 0.0, 0.0], &[false, false, false]);
    let node1 = system.create_node(&vector![a, c, 0.0], &[true, true, false]);
    let node2 = system.create_node(&vector![a + b, 0.0, 0.0], &[false, false, false]);

    let bar01 = system.add_element(&[node0, node1], StringElement::bar(EA, 0.0, f64::hypot(a, c)));
    let bar12 = system.add_element(&[node1, node2], StringElement::bar(EA, 0.0, f64::hypot(b, c)));

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

        let N1_sys = system.element_ref::<StringElement>(bar01).normal_force();
        let N2_sys = system.element_ref::<StringElement>(bar12).normal_force();

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