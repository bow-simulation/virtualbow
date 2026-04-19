// Various nonlinear beams are simulated (static/dynamic/eigen) and compared with
// reference results obtained with GXBeam (https://github.com/byuflowlab/GXBeam.jl)

use nalgebra::Complex;
use std::f64::consts::{FRAC_PI_2, PI, TAU};
use assert2::assert;
use approx::assert_abs_diff_eq;
use std::fs::File;
use std::path::Path;
use serde::Deserialize;
use test_each_file::test_each_path;
use virtualbow_num::fem::elements::beam::beam::BeamElement;
use virtualbow_num::fem::elements::beam::geometry::{CrossSection, PlanarCurve};
use virtualbow_num::fem::elements::beam::linear::LinearBeamSegment;
use virtualbow_num::fem::solvers::dynamics::{DynamicSolver, DynamicSolverSettings, DynamicTolerances, StopCondition, TimeStepping};
use virtualbow_num::fem::solvers::eigen::{natural_frequencies, natural_frequencies_from_eigenvalues};
use virtualbow_num::fem::solvers::statics::{LoadControl, StaticTolerances};
use virtualbow_num::fem::system::dof::DofType;
use virtualbow_num::fem::system::node::Node;
use virtualbow_num::fem::system::system::System;
use virtualbow_num::testutils::curves::{Line, Arc, Wave};
use virtualbow_num::testutils::plotter2::Plotter;
use virtualbow_num::testutils::sections::Section;
use virtualbow_num::utils::newton::NewtonSettings;

#[derive(Deserialize)]
struct Output {
    settings: Settings,
    eigen: Vec<Complex<f64>>,
    statics: Vec<OutputState>,
    dynamics: Vec<OutputState>,
}

#[derive(Deserialize)]
#[allow(dead_code)]
struct Settings {
    n_elements: usize,
    n_eigen: usize,
    n_static: usize,
    n_dynamic: usize,
    i_dynamic: Vec<usize>
}

#[derive(Deserialize)]
#[allow(dead_code)]
struct OutputState {
    lambda: f64,
    time: f64,

    x: Vec<f64>,
    y: Vec<f64>,
    phi: Vec<f64>,

    N: Vec<f64>,
    Q: Vec<f64>,
    M: Vec<f64>,

    epsilon: Vec<f64>,
    gamma: Vec<f64>,
    kappa: Vec<f64>,
}

impl Output {
    // Loads output from msgpack file
    pub fn load(path: &Path) -> Self {
        let file = File::open(path).expect("Failed to open file");
        rmp_serde::from_read(&file).expect("Failed to deserialize")
    }
}

fn create_curve(name: &str) -> Box<dyn PlanarCurve> {
    match name  {
        "curve1" => Box::new(Line::new(0.9)),
        "curve2" => Box::new(Arc::new(0.9, 1.2)),
        "curve3" => Box::new(Wave::new(0.9, 10)),
        _ => unreachable!(),
    }
}

fn create_section(name: &str) -> impl CrossSection {
    match name {
        "section1" => Section::new(800.0, 15e9, 5e9, &[0.01], &[0.01], &[0.0]),
        "section2" => Section::new(800.0, 15e9, 5e9, &[0.01], &[0.01], &[-0.005]),
        "section3" => Section::new(800.0, 15e9, 5e9, &[0.01], &[0.01], &[0.005]),
        "section4" => Section::new(800.0, 15e9, 5e9, &[0.012, 0.006], &[0.012, 0.006], &[0.0, 0.0]),
        "section5" => Section::new(800.0, 15e9, 5e9, &[0.012, 0.006], &[0.012, 0.006], &[-0.006, 0.003]),
        "section6" => Section::new(800.0, 15e9, 5e9, &[0.012, 0.009, 0.012, 0.009], &[0.012, 0.009, 0.012, 0.009], &[0.0, 0.0, 0.0, 0.0]),
        "section7" => Section::new(800.0, 15e9, 5e9, &[0.012, 0.009, 0.012, 0.009], &[0.012, 0.009, 0.012, 0.009], &[-0.006, 0.0045, -0.006, 0.0045]),
        _ => unreachable!(),
    }
}

test_each_path!{ in "virtualbow_num/data/gxbeam" => simulate_and_test_beam}

fn simulate_and_test_beam(path: &Path) {
    // Open reference data from path
    let output = Output::load(path);

    let name = path.file_stem().unwrap();
    let name = name.to_str().unwrap();
    let parts: Vec<&str> = name.split("_").collect();
    assert!(parts.len() == 2);

    // Create curve and section according to filename
    let curve = create_curve(parts[0]);
    let section = create_section(parts[1]);

    let n_elements = 50;                                         // Number of elements in our solution
    assert!(output.settings.n_elements % n_elements == 0);       // Number of reference elements must be divisible by our number
    let n_ref_per_element = output.settings.n_elements /n_elements;    // Number of reference elements in one of ours
    let n_eval_per_element = 2*n_ref_per_element + 1;                  // Number of eval points: One for each reference node and one for each reference element midpoint

    let Fx = -60.0;
    let Fy = 80.0;
    let Mz = 10.0;
    let omega = 200.0;

    let static_tolerances = StaticTolerances::new(curve.length(), FRAC_PI_2, 1e-7);
    let dynamic_tolerances = DynamicTolerances { linear_acc: 1e-7, angular_acc: 1e-6, loadfactor: 1e-7};

    let settings = NewtonSettings {
        max_iterations: 100,
        line_searching: None
    };

    // Create linear beam segments and elements
    let (segments, points, _lengths) = LinearBeamSegment::discretize(&curve, &section, n_elements, n_eval_per_element);

    // Create nodes and elements
    let mut system = System::new();
    let nodes: Vec<Node> = points.iter().enumerate().map(|(i, point)| system.create_node(point, &[DofType::active_if(i != 0); 3])).collect();
    let elements: Vec<usize> = segments.iter().enumerate().map(|(i, segment)| system.add_element(&[nodes[i], nodes[i+1]], BeamElement::new(segment))).collect();

    // Add tip forces
    system.add_force(nodes[n_elements].x(), move |_t| { Fx });
    system.add_force(nodes[n_elements].y(), move |_t| { Fy });
    system.add_force(nodes[n_elements].φ(), move |_t| { Mz });

    let mut plotter = Plotter::new();

    // Compute natural frequencies
    let modes_num = natural_frequencies(&mut system).unwrap();
    let modes_ref = natural_frequencies_from_eigenvalues(&output.eigen).unwrap();

    for i in 0..modes_ref.len() {
        plotter.add_point("01 Modal Frequency", "Mode [-]", "Omega [1/s]", "Actual", 0, (i as f64, modes_num[i].omega));
        plotter.add_point("01 Modal Frequency", "Mode [-]", "Omega [1/s]", "GXBeam", 1, (i as f64, modes_ref[i].omega));

        plotter.add_point("02 Modal Damping", "Mode [-]", "Zeta [-]", "Actual", 0, (i as f64, modes_num[i].zeta));
        plotter.add_point("02 Modal Damping", "Mode [-]", "Zeta [-]", "GXBeam", 1, (i as f64, modes_ref[i].zeta));

        // Plot all reference frequencies, but only check first four for accuracy
        // Higher frequencies start to diverge slightly due to VirtualBow's simpler mass matrix
        if i < 3 {
            assert_abs_diff_eq!(modes_num[i].omega, modes_ref[i].omega, epsilon=2e-2*modes_ref[i].omega);
            assert_abs_diff_eq!(modes_num[i].zeta, modes_ref[i].zeta, epsilon=1e-4);
        }
    }

    // Simulate statics

    let solver = LoadControl::new(&mut system, static_tolerances, settings);

    // Maximum absolute values as reference for numerical tolerances
    let N_max = output.statics.last().unwrap().N.iter().copied().map(f64::abs).fold(f64::NEG_INFINITY, f64::max);
    let Q_max = output.statics.last().unwrap().Q.iter().copied().map(f64::abs).fold(f64::NEG_INFINITY, f64::max);
    let M_max = output.statics.last().unwrap().M.iter().copied().map(f64::abs).fold(f64::NEG_INFINITY, f64::max);
    let ε_max = output.statics.last().unwrap().epsilon.iter().copied().map(f64::abs).fold(f64::NEG_INFINITY, f64::max);
    let γ_max = output.statics.last().unwrap().gamma.iter().copied().map(f64::abs).fold(f64::NEG_INFINITY, f64::max);
    let κ_max = output.statics.last().unwrap().kappa.iter().copied().map(f64::abs).fold(f64::NEG_INFINITY, f64::max);

    let mut iState = 0_usize;    // Index of the current loadstep (0 based)
    solver.solve_equilibrium_path(output.settings.n_static - 1, &mut |system, _eval, _info| {
        // 1. Check positions and angles of the nodes
        for (iNode, &node) in nodes.iter().enumerate() {
            let iRef = n_ref_per_element*iNode;    // Current GXBeam node index

            let x_num = system.get_position(node.x());
            let y_num = system.get_position(node.y());
            let φ_num = system.get_position(node.φ());

            let x_ref = output.statics[iState].x[iRef];
            let y_ref = output.statics[iState].y[iRef];
            let φ_ref = output.statics[iState].phi[iRef];

            plotter.add_point("03 Static Position (Nodes)", "x [m]", "y [m]", &format!("State {iState:02} - Actual"), 0, (x_num, y_num));
            plotter.add_point("03 Static Position (Nodes)", "x [m]", "y [m]", &format!("State {iState:02} - GXBeam"), 1, (x_ref, y_ref));

            plotter.add_point("04 Static Angle (Nodes)", "Index", "Angle [rad]", &format!("State {iState:02} - Actual"), 0, (iNode as f64, φ_num));
            plotter.add_point("04 Static Angle (Nodes)", "Index", "Angle [rad]", &format!("State {iState:02} - GXBeam"), 1, (iNode as f64, φ_ref));

            assert_abs_diff_eq!(x_ref, x_num, epsilon=2e-3*curve.length());
            assert_abs_diff_eq!(y_ref, y_num, epsilon=2e-3*curve.length());
            assert_abs_diff_eq!(φ_ref, φ_num, epsilon=3e-3*TAU);
        }

        // 2. Check evaluated properties within the elements
        for (iElement, &element) in elements.iter().enumerate() {
            let element = system.element_ref::<BeamElement>(element);
            element.eval_properties().enumerate().for_each(|(iEval, eval)| {
                // Compare node results or element midpoint results, depending on index of the eval point
                if iEval % 2 == 0 {
                    let iRef = iElement*n_ref_per_element + iEval/2;    // Current GXBeam node index

                    let x_num = eval.position[0];
                    let y_num = eval.position[1];
                    let φ_num = eval.position[2];

                    let x_ref = output.statics[iState].x[iRef];
                    let y_ref = output.statics[iState].y[iRef];
                    let φ_ref = output.statics[iState].phi[iRef];

                    // Adjust angle by +- 2PI if the difference is too large
                    let φ_num = if (φ_num - φ_ref) > PI {
                        φ_num - TAU
                    } else if φ_num - φ_ref < -PI {
                        φ_num + TAU
                    } else {
                        φ_num
                    };

                    plotter.add_point("05 Static Position (Elements)", "x [m]", "y [m]", &format!("State {iState:02} - Actual"), 0, (x_num, y_num));
                    plotter.add_point("05 Static Position (Elements)", "x [m]", "y [m]", &format!("State {iState:02} - GXBeam"), 1, (x_ref, y_ref));

                    plotter.add_point("06 Static Angle (Elements)", "Index", "Angle [rad]", &format!("State {iState:02} - Actual"), 0, (iRef as f64, φ_num));
                    plotter.add_point("06 Static Angle (Elements)", "Index", "Angle [rad]", &format!("State {iState:02} - GXBeam"), 1, (iRef as f64, φ_ref));

                    assert_abs_diff_eq!(x_ref, x_num, epsilon=2e-3*curve.length());
                    assert_abs_diff_eq!(y_ref, y_num, epsilon=2e-3*curve.length());
                    assert_abs_diff_eq!(φ_ref, φ_num, epsilon=3e-3*TAU);
                }
                else {
                    let iRef = iElement*n_ref_per_element + (iEval - 1)/2;    // Current GXBeam element index

                    let N_num = eval.forces[0];
                    let Q_num = eval.forces[1];
                    let M_num = eval.forces[2];

                    let N_ref = output.statics[iState].N[iRef];
                    let Q_ref = output.statics[iState].Q[iRef];
                    let M_ref = output.statics[iState].M[iRef];

                    let ε_num = eval.strains[0];
                    let γ_num = eval.strains[1];
                    let κ_num = eval.strains[2];

                    let ε_ref = output.statics[iState].epsilon[iRef];
                    let γ_ref = output.statics[iState].gamma[iRef];
                    let κ_ref = output.statics[iState].kappa[iRef];

                    plotter.add_point("07 Static Normal Force (Elements)", "Index", "Force [N]", &format!("State {iState:02} - Actual"), 0, (iRef as f64, N_num));
                    plotter.add_point("07 Static Normal Force (Elements)", "Index", "Force [N]", &format!("State {iState:02} - GXBeam"), 1, (iRef as f64, N_ref));

                    plotter.add_point("08 Static Shear Force (Elements)", "Index", "Force [N]", &format!("State {iState:02} - Actual"), 0, (iRef as f64, Q_num));
                    plotter.add_point("08 Static Shear Force (Elements)", "Index", "Force [N]", &format!("State {iState:02} - GXBeam"), 1, (iRef as f64, Q_ref));

                    plotter.add_point("09 Static Bending Moment (Elements)", "Index", "Moment [Nm]", &format!("State {iState:02} - Actual"), 0, (iRef as f64, M_num));
                    plotter.add_point("09 Static Bending Moment (Elements)", "Index", "Moment [Nm]", &format!("State {iState:02} - GXBeam"), 1, (iRef as f64, M_ref));

                    plotter.add_point("10 Static Longitudinal Strain (Elements)", "Index", "Strain [-]", &format!("State {iState:02} - Actual"), 0, (iRef as f64, ε_num));
                    plotter.add_point("10 Static Longitudinal Strain (Elements)", "Index", "Strain [-]", &format!("State {iState:02} - GXBeam"), 1, (iRef as f64, ε_ref));

                    plotter.add_point("11 Static Shear Strain (Elements)", "Index", "Strain [-]", &format!("State {iState:02} - Actual"), 0, (iRef as f64, γ_num));
                    plotter.add_point("11 Static Shear Strain (Elements)", "Index", "Strain [-]", &format!("State {iState:02} - GXBeam"), 1, (iRef as f64, γ_ref));

                    plotter.add_point("12 Static Bending Curvature (Elements)", "Index", "Strain [-]", &format!("State {iState:02} - Actual"), 0, (iRef as f64, κ_num));
                    plotter.add_point("12 Static Bending Curvature (Elements)", "Index", "Strain [-]", &format!("State {iState:02} - GXBeam"), 1, (iRef as f64, κ_ref));

                    assert_abs_diff_eq!(N_ref, N_num, epsilon=2e-2*N_max);
                    assert_abs_diff_eq!(Q_ref, Q_num, epsilon=2e-2*Q_max);
                    assert_abs_diff_eq!(M_ref, M_num, epsilon=2e-2*M_max);

                    assert_abs_diff_eq!(ε_ref, ε_num, epsilon=2e-2*ε_max);
                    assert_abs_diff_eq!(γ_ref, γ_num, epsilon=2e-2*γ_max);
                    assert_abs_diff_eq!(κ_ref, κ_num, epsilon=2e-2*κ_max);
                }
            });
        }

        iState += 1;

        return true;
    }).unwrap();

    // Simulate dynamics

    let period = TAU/omega;
    let timestep = period/((output.settings.n_dynamic - 1) as f64);

    system.reset_state();
    system.reset_forces();
    system.add_force(nodes[n_elements].x(), move |t| { f64::sin(omega*t)*Fx });
    system.add_force(nodes[n_elements].y(), move |t| { f64::sin(omega*t)*Fy });
    system.add_force(nodes[n_elements].φ(), move |t| { f64::sin(omega*t)*Mz });

    let settings = DynamicSolverSettings { time_stepping: TimeStepping::Fixed(timestep), ..Default::default() };
    let mut solver = DynamicSolver::new(&mut system, dynamic_tolerances, settings);

    let mut iState = 1_usize;    // Index of the current timestep (1 based)
    solver.solve(StopCondition::Time(period), &mut |system, _eval| {
        if let Some(iEval) = output.settings.i_dynamic.iter().position(|&x| x == iState) {
            // Verify that the time points match
            let t_num = system.get_time();
            let t_ref = output.dynamics[iEval].time;
            assert_abs_diff_eq!(t_num, t_ref, epsilon=1e-12);

            // 1. Check positions and angles of the nodes
            // Due to VirtualBow's lumped mass matrix, the dynamic results for off-center cross-sections deviate slightly.
            // That's why the numerical tolerances are larger than in the static case.
            for (iNode, &node) in nodes.iter().enumerate() {
                let iRef = n_ref_per_element * iNode;    // Current GXBeam node index

                let x_num = system.get_position(node.x());
                let y_num = system.get_position(node.y());
                let φ_num = system.get_position(node.φ());

                let x_ref = output.dynamics[iEval].x[iRef];
                let y_ref = output.dynamics[iEval].y[iRef];
                let φ_ref = output.dynamics[iEval].phi[iRef];

                plotter.add_point("13 Dynamic Position (Nodes)", "x [m]", "y [m]", &format!("State {iState:02} - Actual"), 0, (x_num, y_num));
                plotter.add_point("13 Dynamic Position (Nodes)", "x [m]", "y [m]", &format!("State {iState:02} - GXBeam"), 1, (x_ref, y_ref));

                plotter.add_point("14 Dynamic Angle (Nodes)", "Index", "Angle [rad]", &format!("State {iState:02} - Actual"), 0, (iNode as f64, φ_num));
                plotter.add_point("14 Dynamic Angle (Nodes)", "Index", "Angle [rad]", &format!("State {iState:02} - GXBeam"), 1, (iNode as f64, φ_ref));

                assert_abs_diff_eq!(x_ref, x_num, epsilon=1e-2*curve.length());
                assert_abs_diff_eq!(y_ref, y_num, epsilon=1e-2*curve.length());
                assert_abs_diff_eq!(φ_ref, φ_num, epsilon=3e-2*TAU);
            }
        }

        iState += 1;

        return true;
    }).unwrap();
}