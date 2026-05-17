// A nonlinear cantilever beam is simulated with an increasing number of elements
// to show the convergence characteristics of the elements.

use std::f64::consts::TAU;
use virtualbow_fem::elements::beam::beam::BeamElement;
use virtualbow_fem::elements::beam::linear::LinearBeamSegment;
use virtualbow_fem::solvers::dynamics::{DynamicSolver, DynamicSolverSettings, DynamicTolerances, StopCondition, TimeStepping};
use virtualbow_fem::solvers::eigen::natural_frequencies;
use virtualbow_fem::solvers::statics::{LoadControl, StaticTolerances};
use virtualbow_fem::system::dof::DofType;
use virtualbow_fem::system::node::Node;
use virtualbow_fem::system::system::System;
use virtualbow_fem::testutils::curves::Line;
use virtualbow_fem::testutils::plotter::Plotter;
use virtualbow_fem::testutils::sections::Section;
use virtualbow_num::newton::NewtonSettings;

// Common solver tolerances and settings
const STATIC_TOLERANCES: StaticTolerances = StaticTolerances { linear_pos: 1e-8, angular_pos: 1e-8, loadfactor: 1e-8 };
const DYNAMIC_TOLERANCES: DynamicTolerances = DynamicTolerances { linear_acc: 1e-7, angular_acc: 1e-6, loadfactor: 1e-7 };
const SETTINGS: NewtonSettings = NewtonSettings { max_iterations: 100, line_searching: None };

#[test]
fn cantilever_eigen_convergence() {
    // Solves for the eigenfrequencies of the beam "curve1" + "section1" from the GXBeam tests for an increasing number of elements.
    // Plots the error in the first three frequencies over the number of elements. This test shows how effective the elements are,
    // i.e. how fast the solution converges and also how many elements the solver can handle.

    let mut plotter = Plotter::new();

    for n_elements in [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 30, 35, 40, 45, 50, 100, 250] {
        let (error_0, error_1, error_2) = solve_straight_uniform_cantilever_eigen(n_elements);

        // Plot the errors over the number of elements up to a certain number (plot becomes boring)
        if n_elements <= 50 {
            plotter.add_point("1st Natural Frequency", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_0));
            plotter.add_point("2nd Natural Frequency", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_1));
            plotter.add_point("3rd Natural Frequency", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_2));
        }

        // Start checking the errors after a certain number of elements
        if n_elements >= 25 {
            assert!(f64::abs(error_0) < 1e-2);
            assert!(f64::abs(error_1) < 1e-2);
            assert!(f64::abs(error_2) < 1e-2);
        }
    }
}


#[test]
fn cantilever_static_convergence() {
    // Solves the statics of the beam "curve1" + "section1" from the GXBeam tests for an increasing number of elements.
    // Plots the error in the tip position and angle over the number of elements. This test shows how effective the elements are,
    // i.e. how fast the solution converges and also how many elements the solver can handle.

    let mut plotter = Plotter::new();

    for n_elements in [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 30, 35, 40, 45, 50, 100, 250] {
        let (error_x, error_y, error_φ) = solve_straight_uniform_cantilever_statics(n_elements);

        // Plot the errors over the number of elements up to a certain number (plot becomes boring)
        if n_elements <= 50 {
            plotter.add_point("Tip Displacement X", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_x));
            plotter.add_point("Tip Displacement Y", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_y));
            plotter.add_point("Tip Rotation Angle", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_φ));
        }

        // Start checking the errors after a certain number of elements
        if n_elements >= 25 {
            assert!(f64::abs(error_x) < 1e-3);
            assert!(f64::abs(error_y) < 1e-3);
            assert!(f64::abs(error_φ) < 1e-3);
        }
    }
}

#[test]
fn cantilever_dynamic_convergence() {
    // Solves the dynamics of the beam "curve1" + "section1" from the GXBeam tests for an increasing number of elements.
    // Plots the error in the final tip position and angle over the number of elements. This test shows how effective the elements are,
    // i.e. how fast the solution converges and also how many elements the solver can handle.

    let mut plotter = Plotter::new();

    for n_elements in [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 30, 35, 40, 45, 50, 100] {
        let (error_x, error_y, error_φ) = solve_straight_uniform_cantilever_dynamics(n_elements);

        // Plot the errors over the number of elements up to a certain number (plot becomes boring)
        if n_elements <= 50 {
            plotter.add_point("Tip Displacement X", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_x));
            plotter.add_point("Tip Displacement Y", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_y));
            plotter.add_point("Tip Rotation Angle", "N Elements", "Error [%]", "Actual", (n_elements, 100.0*error_φ));
        }

        // Start checking the errors after a certain number of elements
        if n_elements >= 25 {
            assert!(f64::abs(error_x) < 5e-3);
            assert!(f64::abs(error_y) < 5e-3);
            assert!(f64::abs(error_φ) < 5e-3);
        }
    }
}

fn solve_straight_uniform_cantilever_eigen(n_elements: usize) -> (f64, f64, f64) {
    // Beam model
    let curve = Line::new(0.9);                                                // TODO: Get from GXBeam tests
    let section = Section::new(800.0, 15e9, 5e9, &[0.01], &[0.01], &[0.0]);    // TODO: Get from GXBeam tests
    let (segments, points, _lengths) = LinearBeamSegment::discretize(&curve, &section, n_elements, 0);

    let mut system = System::new();
    let nodes: Vec<Node> = points.iter().enumerate().map(|(i, &point)| system.create_node(point, [DofType::active_if(i != 0); 3])).collect();
    segments.iter().enumerate().for_each(|(i, segment)| {
        system.add_element(&[nodes[i], nodes[i+1]], BeamElement::new(segment));
    });

    // Compute numerical solution
    let modes_num = natural_frequencies(&mut system).unwrap();
    let omega0_num = modes_num[0].omega;
    let omega1_num = modes_num[1].omega;
    let omega2_num = modes_num[2].omega;

    // Reference solution
    // TODO: Get from gxbeam data
    let omega0_ref = 54.25432389116355;
    let omega1_ref = 339.8131715812344;
    let omega2_ref = 950.6170530567713;

    (
        (omega0_num - omega0_ref)/omega0_ref,
        (omega1_num - omega1_ref)/omega1_ref,
        (omega2_num - omega2_ref)/omega2_ref
    )
}

fn solve_straight_uniform_cantilever_statics(n_elements: usize) -> (f64, f64, f64) {
    let Fx = -60.0;
    let Fy = 80.0;
    let Mz = 10.0;

    // Beam model
    let curve = Line::new(0.9);                                                // TODO: Get from GXBeam tests
    let section = Section::new(800.0, 15e9, 5e9, &[0.01], &[0.01], &[0.0]);    // TODO: Get from GXBeam tests
    let (segments, points, _lengths) = LinearBeamSegment::discretize(&curve, &section, n_elements, 0);

    let mut system = System::new();
    let nodes: Vec<Node> = points.iter().enumerate().map(|(i, &point)| system.create_node(point, [DofType::active_if(i != 0); 3])).collect();
    segments.iter().enumerate().for_each(|(i, segment)| {
        system.add_element(&[nodes[i], nodes[i+1]], BeamElement::new(segment));
    });

    // Compute numerical solution
    system.add_force(nodes[n_elements].x(), move |_t| { Fx });
    system.add_force(nodes[n_elements].y(), move |_t| { Fy });
    system.add_force(nodes[n_elements].φ(), move |_t| { Mz });

    let solver = LoadControl::new(&mut system, STATIC_TOLERANCES, SETTINGS);
    solver.solve_equilibrium().unwrap();

    // TODO: Introduce get_positions/get_displacements(&Node) -> [f64; 3]?
    let tip_node = nodes.last().unwrap();
    let x_num = system.get_dof_position(tip_node.x());
    let y_num = system.get_dof_position(tip_node.y());
    let φ_num = system.get_dof_position(tip_node.φ());

    // Reference solution
    // TODO: Get from gxbeam data
    let x_ref = 0.11681995638419107;
    let y_ref = 0.7430334286730679;
    let φ_ref = 2.1116517121658345;

    (
        (x_num - x_ref)/x_ref,
        (y_num - y_ref)/y_ref,
        (φ_num - φ_ref)/φ_ref
    )
}

fn solve_straight_uniform_cantilever_dynamics(n_elements: usize) -> (f64, f64, f64) {
    let Fx = -60.0;
    let Fy = 80.0;
    let Mz = 10.0;
    let omega = 200.0;

    // Beam model
    let curve = Line::new(0.9);                                                // TODO: Get from GXBeam tests
    let section = Section::new(800.0, 15e9, 5e9, &[0.01], &[0.01], &[0.0]);    // TODO: Get from GXBeam tests
    let (segments, points, _lengths) = LinearBeamSegment::discretize(&curve, &section, n_elements, 0);

    let mut system = System::new();
    let nodes: Vec<Node> = points.iter().enumerate().map(|(i, &point)| system.create_node(point, [DofType::active_if(i != 0); 3])).collect();
    segments.iter().enumerate().for_each(|(i, segment)| {
        system.add_element(&[nodes[i], nodes[i+1]], BeamElement::new(segment));
    });


    let period = TAU/omega;
    let timestep = period/((250 - 1) as f64);

    // Compute numerical solution
    system.add_force(nodes[n_elements].x(), move |t| { f64::sin(omega*t)*Fx });
    system.add_force(nodes[n_elements].y(), move |t| { f64::sin(omega*t)*Fy });
    system.add_force(nodes[n_elements].φ(), move |t| { f64::sin(omega*t)*Mz });

    let settings = DynamicSolverSettings { time_stepping: TimeStepping::Fixed(timestep), ..Default::default() };
    let mut solver = DynamicSolver::new(&mut system, DYNAMIC_TOLERANCES, settings);
    solver.solve(StopCondition::Time(period), &mut |_, _| true).unwrap();

    // TODO: Introduce get_positions/get_displacements(&Node) -> [f64; 3]?
    let tip_node = nodes.last().unwrap();
    let x_num = system.get_dof_position(tip_node.x());
    let y_num = system.get_dof_position(tip_node.y());
    let φ_num = system.get_dof_position(tip_node.φ());

    // Reference solution
    // TODO: Get from gxbeam data
    let x_ref = 0.679028977782441;
    let y_ref = 0.5150970196235944;
    let φ_ref = 1.0601290645250199;

    (
        (x_num - x_ref)/x_ref,
        (y_num - y_ref)/y_ref,
        (φ_num - φ_ref)/φ_ref
    )
}