use std::f64::consts::FRAC_PI_2;
use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::vector;
use crate::fem::system::system::System;
use crate::fem::elements::string::StringElement;
use crate::fem::solvers::statics::StaticSolver;
use crate::numerics::newton::NewtonSettings;
use crate::tests::utils;
use crate::tests::utils::plotter::Plotter;

// Tests the string element as part of a static system

#[test]
fn string_over_quarter_circle() {
    // Contact of the string element with a fixed quarter-arc

    let r = 1.5;
    let n = 100;
    let l0 = 0.5*r;
    let EA = 2100.0;

    let mut system = System::new();

    // Create fixed nodes in a circular arc from {x=r, y=0} to {x=0, y=r}
    let fixed_nodes = lin_space(0.0..=FRAC_PI_2, n).map(|alpha| {
        system.create_node(&vector![r*f64::cos(alpha), r*f64::sin(alpha), 0.0], &[false; 3])
    }).collect_vec();

    // Create end node for the string that is free in y direction
    let free_node = system.create_node(&vector![r, r, 0.0], &[false, true, false]);
    system.add_force(free_node.y(), |_t| { 1.0 });

    // String nodes are the free node, followed by the fixed ones
    let mut string_nodes = vec![free_node];
    string_nodes.extend_from_slice(&fixed_nodes);

    // Create and add string element. No offsets in this example.
    let offsets = vec![0.0; string_nodes.len()];
    let element = system.add_element(&string_nodes, StringElement::new(EA, 0.0, l0, 1.0, offsets));

    utils::checks::check_system_invariants(&mut system);

    let mut plotter = Plotter::new();
    let mut solver = StaticSolver::new(&mut system, NewtonSettings::default());

    let result = solver.equilibrium_path_displacement_controlled(free_node.y(), 0.0, 100, &mut |system, statics, _| {
        let y = system.get_displacement(free_node.y());
        let h = r - y;

        let points = system.element_ref::<StringElement>(element).contact_positions().collect_vec();

        println!("Points: {}", points.len());

        // Actual contact angle vs analytical reference
        let alpha_num = f64::atan2(points[1][1], points[1][0]);
        let alpha_ref = 2.0*f64::atan(1.0 - h/r);

        plotter.add_point((h, alpha_num), (h, alpha_ref), "Contact angle", "h [m]", "α [rad]");
        assert_abs_diff_eq!(alpha_num, alpha_ref, epsilon=1e-1);    // Expectedly not very accurate due to the finite number of contact points

        // Actual string length vs analytical reference
        let l_num = system.element_ref::<StringElement>(element).get_current_length();
        let l_ref = (FRAC_PI_2 - alpha_ref + f64::tan(alpha_ref/2.0))*r;

        plotter.add_point((h, l_num), (h, l_ref), "String length", "h [m]", "l [m]");
        assert_abs_diff_eq!(l_num, l_ref, epsilon=1e-4*l0);

        // Actual node force vs analytical reference
        let F_num = statics.get_scaled_external_force(free_node.y());
        let F_ref = EA/l0*(l_ref - l0)*(1.0/(1.0 + f64::cos(alpha_ref)) - 1.0)*2.0*r*r/(h*h - 2.0*h*r + 2.0*r*r);

        plotter.add_point((h, l_num), (h, l_ref), "Node force", "h [m]", "F [m]");
        assert_relative_eq!(F_num, F_ref, max_relative=1e-2);

        // Actual string force vs analytical reference
        let N_num = system.element_ref::<StringElement>(element).normal_force_total();
        let N_ref = EA/l0*(l_ref - l0);

        plotter.add_point((h, N_num), (h, N_ref), "String force", "h [m]", "N [m]");
        assert_relative_eq!(N_num, N_ref, max_relative=1e-2);

        true
    });

    assert!(result.is_ok());
}
