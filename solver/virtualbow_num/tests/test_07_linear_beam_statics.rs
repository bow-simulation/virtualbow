use approx::assert_abs_diff_eq;
use virtualbow_num::testutils::plotter::Plotter;
use virtualbow_num::fem::elements::beam::beam::BeamElement;
use virtualbow_num::fem::elements::beam::linear::LinearBeamSegment;
use virtualbow_num::fem::solvers::statics::{LoadControl, StaticTolerances};
use virtualbow_num::fem::system::dof::DofType;
use virtualbow_num::fem::system::node::Node;
use virtualbow_num::fem::system::system::System;
use virtualbow_num::testutils::curves::Line;
use virtualbow_num::testutils::sections::Section;
use virtualbow_num::utils::newton::NewtonSettings;

// Linear beam problems are solved and the results compared to analytical reference solutions.

// Common solver tolerances and settings
const TOLERANCES: StaticTolerances = StaticTolerances { linear_pos: 1e-8, angular_pos: 1e-8, loadfactor: 1e-8 };
const SETTINGS: NewtonSettings = NewtonSettings { max_iterations: 100, line_searching: None };

#[test]
fn straight_uniform_elongation() {
    // A straight beam with uniform cross-section is elongated by a normal force in the direction of the beam axis.
    // There are no nonlinearities in this setup, so that we can compare the resulting elongation to the analytical solution.
    // All other displacements and forces (bending, shear) must be close to zero.

    // Beam parameters
    let l = 0.9;
    let w = 0.01;
    let h = 0.01;
    let F = 10.0;

    let E = 210e9;
    let G = 80e9;
    let ρ = 7850.0;

    let n_elements = 25;
    let n_eval = 5;

    // Beam model
    let curve = Line::new(l);
    let section = Section::new(ρ, E, G, &[w], &[h], &[0.0]);
    let (segments, points, _lengths) = LinearBeamSegment::discretize(&curve, &section, n_elements, n_eval);

    let mut system = System::new();
    let nodes: Vec<Node> = points.iter().enumerate().map(|(i, point)| system.create_node(point, &[DofType::active_if(i != 0); 3])).collect();
    let elements: Vec<usize> = segments.iter().enumerate().map(|(i, segment)| system.add_element(&[nodes[i], nodes[i+1]], BeamElement::new(segment))).collect();

    // Compute numerical solution
    system.add_force(nodes[n_elements].x(), move |_t| { F });
    let solver = LoadControl::new(&mut system, TOLERANCES, SETTINGS);
    solver.solve_equilibrium().unwrap();

    // Reference cross-section stiffnesses
    let A = w*h;
    let EA = E*A;

    let mut plotter = Plotter::new();

    for element in elements {
        let element = system.element_ref::<BeamElement>(element);
        element.eval_properties().for_each(|eval| {
            // Numerical solution for displacements and forces
            let u_num = eval.position[0] - eval.length;
            let x_num = eval.position[0];
            let y_num = eval.position[1];
            let φ_num = eval.position[2];
            let N_num = eval.forces[0];
            let Q_num = eval.forces[1];
            let M_num = eval.forces[2];

            // Analytical reference solution
            let u_ref = F/EA*x_num;
            let y_ref = 0.0;
            let φ_ref = 0.0;
            let N_ref = F;
            let Q_ref = 0.0;
            let M_ref = 0.0;

            plotter.add_point((x_num, u_num), (x_num, u_ref), "01 Elongation", "x [m]", "u [m]");
            plotter.add_point((x_num, N_num), (x_num, N_ref), "02 Normal Force", "x [m]", "Force [N]");
            plotter.add_point((x_num, N_num), (x_num, N_ref), "03 Normal Force", "x [m]", "Force [N]");
            plotter.add_point((x_num, Q_num), (x_num, Q_ref), "04 Shear Force", "x [m]", "Force [N]");
            plotter.add_point((x_num, M_num), (x_num, M_ref), "05 Bending Moment", "x [m]", "Moment [Nm]");

            assert_abs_diff_eq!(u_num, u_ref, epsilon=1e-9);
            assert_abs_diff_eq!(y_num, y_ref, epsilon=1e-9);
            assert_abs_diff_eq!(φ_num, φ_ref, epsilon=1e-9);
            assert_abs_diff_eq!(N_num, N_ref, epsilon=1e-7);
            assert_abs_diff_eq!(Q_num, Q_ref, epsilon=1e-9);
            assert_abs_diff_eq!(M_num, M_ref, epsilon=1e-9);
        });
    }
}

#[test]
fn straight_uniform_cantilever() {
    // A straight beam with uniform cross-section is bent by a transversal force placed at the beam tip while the other end is clamped.
    // The force is chosen small enough such that the problem can be treated as approximately linear and the results can be
    // compared to the analytical solution of the cantilever beam according to the Timoshenko beam theory.
    // The beam parameters are chosen such that there is some significant shear effect.

    // Beam parameters
    let l = 0.9;
    let w = 0.01;
    let h = 0.01;
    let F = 0.1;

    let E = 210e9;
    let G = 80e9;
    let ρ = 7850.0;

    let n_elements = 25;
    let n_eval = 5;

    // Beam model
    let curve = Line::new(l);
    let section = Section::new(ρ, E, G, &[w], &[h], &[0.0]);
    let (segments, points, _lengths) = LinearBeamSegment::discretize(&curve, &section, n_elements, n_eval);

    let mut system = System::new();
    let nodes: Vec<Node> = points.iter().enumerate().map(|(i, point)| system.create_node(point, &[DofType::active_if(i != 0); 3])).collect();
    let elements: Vec<usize> = segments.iter().enumerate().map(|(i, segment)| system.add_element(&[nodes[i], nodes[i+1]], BeamElement::new(segment))).collect();

    // Compute numerical solution
    system.add_force(nodes[n_elements].y(), move |_t| { F });
    let solver = LoadControl::new(&mut system, TOLERANCES, SETTINGS);
    solver.solve_equilibrium().unwrap();

    // Reference cross-section stiffnesses
    let I = w*h.powi(3)/12.0;
    let A = w*h;
    let EI = E*I;
    let GA = G*A;

    let mut plotter = Plotter::new();

    for element in elements {
        let element = system.element_ref::<BeamElement>(element);
        element.eval_properties().for_each(|eval| {
            // Numerical solution for displacements and forces
            let x_num = eval.position[0];
            let y_num = eval.position[1];
            let φ_num = eval.position[2];
            let N_num = eval.forces[0];
            let Q_num = eval.forces[1];
            let M_num = eval.forces[2];


            // Analytical reference solution (https://en.wikipedia.org/wiki/Timoshenko%E2%80%93Ehrenfest_beam_theory)
            let x_ref = x_num;
            let y_ref = F/GA*x_ref - F/EI*x_ref.powi(2)*(x_ref/6.0 - l/2.0);
            let φ_ref = F/EI*x_ref*(l - x_ref/2.0);
            let N_ref = 0.0;
            let Q_ref = F;
            let M_ref = F*(l - x_ref);

            plotter.add_point((x_num, y_num), (x_ref, y_ref), "01 Deflection", "x [m]", "y [m]");
            plotter.add_point((x_num, φ_num), (x_ref, φ_ref), "02 Section Angle", "x [m]", "φ [rad]");
            plotter.add_point((x_num, N_num), (x_ref, N_ref), "03 Normal Force", "x [m]", "Force [N]");
            plotter.add_point((x_num, Q_num), (x_ref, Q_ref), "04 Shear Force", "x [m]", "Force [N]");
            plotter.add_point((x_num, M_num), (x_ref, M_ref), "05 Bending Moment", "x [m]", "Moment [Nm]");

            assert_abs_diff_eq!(x_num, x_ref, epsilon=1e-9);
            assert_abs_diff_eq!(y_num, y_ref, epsilon=1e-9);
            assert_abs_diff_eq!(φ_num, φ_ref, epsilon=1e-9);
            assert_abs_diff_eq!(N_num, N_ref, epsilon=4e-5);
            assert_abs_diff_eq!(Q_num, Q_ref, epsilon=1e-5);
            assert_abs_diff_eq!(M_num, M_ref, epsilon=1e-5);
        });
    }
}