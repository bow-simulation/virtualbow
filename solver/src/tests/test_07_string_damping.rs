use std::f64::consts::PI;
use iter_num_tools::lin_space;
use nalgebra::vector;
use crate::fem::elements::mass::MassElement;
use crate::fem::elements::string::StringElement;
use crate::fem::solvers::eigen::natural_frequencies;
use crate::fem::system::node::{Constraints, Node};
use crate::fem::system::system::System;
use crate::tests::utils::plotter::Plotter;

#[test]
fn verify_analytic_damping_ratio() {
    // This test verifies the analytical solution for the natural frequencies and damping ratios of a viscously damped continuous bar
    // as derived in the theory manual. This is done by comparing the analytical results against a finite element system of bar elements.

    // Parameters
    let ρA = 0.01;
    let ηA = 1.0;
    let EA = 1e4;
    let L = 1.5;

    let n = 100;

    // Finite element system
    let mut system = System::new();
    let mut nodes = Vec::<Node>::new();

    let lengths: Vec<f64> = lin_space(0.0..=L, n+2).collect();
    for (i, &s) in lengths.iter().enumerate() {
        let constraints = if s != 0.0 {
            Constraints::x_pos_free()
        } else {
            Constraints::all_fixed()
        };
        nodes.push(system.create_node(&vector![s, 0.0, 0.0], constraints));
    }

    // Add bar elements between nodes
    for i in 0..nodes.len()-1 {
        let l = lengths[i+1] - lengths[i];
        system.add_element(&[nodes[i], nodes[i+1]], StringElement::bar(EA, ηA, l));
    }

    // Add mass elements at nodes
    for i in 0..nodes.len() {
        let m = ρA*L/(nodes.len() as f64);
        system.add_element(&[nodes[i]], MassElement::new(m));
    }

    // Compute eigen-modes of the system and compare with analytical solution
    let modes = natural_frequencies(&mut system).unwrap();

    let mut plotter = Plotter::new();
    for (i, mode) in modes.iter().enumerate() {
        let k = i + 1;
        let omega_ref = PI*((2*k - 1) as f64)/(2.0*L)*f64::sqrt(EA/ ρA);
        let zeta_ref = PI*((2*k - 1) as f64)/(4.0*L)*ηA /f64::sqrt(ρA *EA);

        plotter.add_point((i as f64, mode.omega), (i as f64, omega_ref), "Modal Frequency", "Mode [-]", "Omega [1/s]");
        plotter.add_point((i as f64, mode.zeta), (i as f64, zeta_ref), "Modal Damping", "Mode [-]", "Zeta [-]");

        assert_relative_eq!(mode.omega, omega_ref, max_relative=1e-2);
        assert_relative_eq!(mode.zeta, zeta_ref, max_relative=1e-2);
    }
}