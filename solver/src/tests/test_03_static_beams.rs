use std::f64::consts::{FRAC_PI_4, TAU};
use nalgebra::DVector;
use crate::bow::sections::section::LayerAlignment;
use crate::bow::model::{BowModel, Layer, Material, Profile, Width};
use crate::bow::profile::input::SegmentInput;
use crate::bow::profile::segments::clothoid::{ArcInput, LineInput};
use crate::bow::simulation::Simulation;
use crate::numerics::functions::normalize_angle;
use crate::tests::utils::plotter::Plotter;

// Various bending problems with increasing complexity are solved and the results compared to analytical or numerical reference solutions.
// Naming of the test cases:
// - linear/nonlinear: Whether the problem is treated as (geometrically) linear or not.
// - straight/curved: Whether the beam is straight or curved in the initial configuration
// - uniform/tapered: Whether the cross section properties change over the length of the beam
// - type of loading:
//   - elongation: Normal force applied to the beam tip, other end clamped
//   - cantilever: Force with a transverse component applied to the beam tip, other end clamped
//   - coilip: Bending moment applied at the beam tip, leading to a pure bending deformation

#[test]
fn linear_straight_uniform_elongation() {
    // A straight beam with uniform cross section is elongated by a normal force in the direction of the beam axis.
    // There are no nonlinearities in this setup, so that we can compare the resulting elongation to the analytical solution.
    // All other displacements and forces (bending, shear) must be close to zero.

    // Beam parameters
    let l = 0.90;
    let w = 0.01;
    let h = 0.01;
    let F = 10.0;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    // Beam bow
    let mut model = BowModel::default();
    model.settings.n_limb_elements = 25;
    model.settings.n_limb_eval_points = 100;
    model.settings.n_draw_steps = 5;
    model.materials = vec![Material::new("", "", rho, E, G)];
    model.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Line(LineInput::new(l))]);
    model.width = Width::new(vec![[0.0, w], [1.0, w]]);
    model.layers = vec![Layer::new("", 0, vec![[0.0, h], [1.0, h]])];

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_load(&model, F, 0.0, 0.0).unwrap();

    // Extract cross section stiffnesses
    let C = setup.limb.stiffness[0];
    let EA = C[(0, 0)];

    let mut plotter = Plotter::new();
    for i in 0..setup.limb.length.len() {
        // Numerical solution for displacements and forces
        let u_num = state.limb_pos[i][0] - setup.limb.length[i];
        let x_num = state.limb_pos[i][0];
        let y_num = state.limb_pos[i][1];
        let φ_num = state.limb_pos[i][2];
        let N_num = state.limb_force[i][0];
        let M_num = state.limb_force[i][1];
        let Q_num = state.limb_force[i][2];

        // Analytical reference solution
        let u_ref = F/EA*x_num;
        let y_ref = 0.0;
        let φ_ref = 0.0;
        let N_ref = F;
        let M_ref = 0.0;
        let Q_ref = 0.0;

        plotter.add_point((x_num, u_num), (x_num, u_ref), "01 Elongation", "x [m]", "u [m]");
        plotter.add_point((x_num, N_num), (x_num, N_ref), "02 Normal Force", "x [m]", "Force [N]");
        plotter.add_point((x_num, N_num), (x_num, N_ref), "03 Normal Force", "x [m]", "Force [N]");
        plotter.add_point((x_num, M_num), (x_num, M_ref), "04 Bending Moment", "x [m]", "Moment [Nm]");
        plotter.add_point((x_num, Q_num), (x_num, Q_ref), "05 Shear Force", "x [m]", "Force [N]");

        assert_abs_diff_eq!(u_num, u_ref, epsilon=1e-9);
        assert_abs_diff_eq!(y_num, y_ref, epsilon=1e-9);
        assert_abs_diff_eq!(φ_num, φ_ref, epsilon=1e-9);
        assert_abs_diff_eq!(N_num, N_ref, epsilon=1e-7);
        assert_abs_diff_eq!(M_num, M_ref, epsilon=1e-9);
        assert_abs_diff_eq!(Q_num, Q_ref, epsilon=1e-9);
    }
}

#[test]
fn linear_straight_uniform_cantilever() {
    // A straight beam with uniform cross section is bent by a transversal force placed at the beam tip while the other end is clamped.
    // The force is chosen small enough such that the problem can be treated as approximately linear and the results can be
    // compared to the analytical solution of the cantilever beam according to the Timoshenko beam theory.
    // The beam parameters are chosen such that there is some significant shear effect.

    // Beam parameters
    let l = 0.9;
    let w = 0.3;
    let h = 0.3;
    let F = 1.0;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    // Beam bow
    let mut model = BowModel::default();
    model.settings.n_limb_elements = 25;
    model.settings.n_limb_eval_points = 100;
    model.settings.n_draw_steps = 5;
    model.materials = vec![Material::new("", "", rho, E, G)];
    model.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Line(LineInput::new(l))]);
    model.width = Width::new(vec![[0.0, w], [1.0, w]]);
    model.layers = vec![Layer::new("", 0, vec![[0.0, h], [1.0, h]])];

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_load(&model, 0.0, F, 0.0).unwrap();

    // Extract cross section stiffnesses
    let C = setup.limb.stiffness[0];
    let EI = C[(1, 1)];
    let GA = C[(2, 2)];

    let mut plotter = Plotter::new();
    for i in 0..setup.limb.length.len() {
        // Numerical solution for displacements and forces
        let x_num = state.limb_pos[i][0];
        let y_num = state.limb_pos[i][1];
        let φ_num = state.limb_pos[i][2];
        let N_num = state.limb_force[i][0];
        let M_num = state.limb_force[i][1];
        let Q_num = state.limb_force[i][2];

        // Analytical reference solution (https://en.wikipedia.org/wiki/Timoshenko%E2%80%93Ehrenfest_beam_theory)
        let x_ref = setup.limb.length[i];
        let y_ref = F/GA* x_num - F/EI* x_num.powi(2)*(x_num/6.0 - l/2.0);
        let φ_ref = F/EI* x_num*(l - x_num/2.0);
        let N_ref = 0.0;
        let M_ref = F*(l - x_num);
        let Q_ref = F;

        plotter.add_point((x_num, y_num), (x_num, y_ref), "01 Deflection", "x [m]", "y [m]");
        plotter.add_point((x_num, φ_num), (x_num, φ_ref), "02 Section Angle", "x [m]", "φ [rad]");
        plotter.add_point((x_num, N_num), (x_num, N_ref), "03 Normal Force", "x [m]", "Force [N]");
        plotter.add_point((x_num, M_num), (x_num, M_ref), "04 Bending Moment", "x [m]", "Moment [Nm]");
        plotter.add_point((x_num, Q_num), (x_num, Q_ref), "05 Shear Force", "x [m]", "Force [N]");

        assert_abs_diff_eq!(x_num, x_ref, epsilon=1e-9);
        assert_abs_diff_eq!(y_num, y_ref, epsilon=1e-9);
        assert_abs_diff_eq!(φ_num, φ_ref, epsilon=1e-9);
        assert_abs_diff_eq!(N_num, N_ref, epsilon=1e-5);
        assert_abs_diff_eq!(M_num, M_ref, epsilon=1e-5);
        assert_abs_diff_eq!(Q_num, Q_ref, epsilon=1e-5);
    }
}

#[test]
fn linear_straight_uniform_coilup() {
    // A straight beam with uniform cross section is subjected to a bending moment at its free end.
    // Even though the problem is nonlinear and the deformations large, the analytical solution of this case
    // is known - the beam is deformed into a circular arch and eventually into a full circle.
    // The mode of deformation is pure bending, shear- and normal forces are zero.

    // Beam parameters
    let l = 0.90;
    let w = 0.01;
    let h = 0.01;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    // Beam bow
    let mut model = BowModel::default();
    model.settings.n_limb_elements = 50;
    model.settings.n_limb_eval_points = 100;
    model.settings.n_draw_steps = 10;
    model.materials = vec![Material::new("", "", rho, E, G)];
    model.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Line(LineInput::new(l))]);
    model.width = Width::new(vec![[0.0, w], [1.0, w]]);
    model.layers = vec![Layer::new("", 0, vec![[0.0, h], [1.0, h]])];

    // Required radius and bending moment
    let I = w*h.powi(3)/12.0;
    let R = l/TAU;
    let M = E*I/R;

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_load(&model, 0.0, 0.0, M).unwrap();

    let mut plotter = Plotter::new();
    for i in 0..setup.limb.length.len() {
        // Numerical solution for displacements and forces
        let s_num = setup.limb.length[i];
        let x_num = state.limb_pos[i][0];
        let y_num = state.limb_pos[i][1];
        let φ_num = normalize_angle(state.limb_pos[i][2]);  // TODO: Normalize internally?
        let N_num = state.limb_force[i][0];
        let M_num = state.limb_force[i][1];
        let Q_num = state.limb_force[i][2];

        // Analytical reference solution
        let φ_ref = normalize_angle(s_num/R);
        let x_ref = R*f64::sin(φ_ref);
        let y_ref = R*(1.0 - f64::cos(φ_ref));
        let N_ref = 0.0;
        let Q_ref = 0.0;
        let M_ref = M;

        plotter.add_point((x_num, y_num), (x_ref, y_ref), "01 Bending Line", "Length [m]", "y [m]");
        plotter.add_point((s_num, φ_num), (s_num, φ_ref), "02 Section Angle", "Length [m]", "φ [rad]");
        plotter.add_point((s_num, N_num), (x_num, N_ref), "03 Normal Force", "x [m]", "Force [N]");
        plotter.add_point((s_num, M_num), (x_num, M_ref), "04 Bending Moment", "x [m]", "Moment [Nm]");
        plotter.add_point((s_num, Q_num), (x_num, Q_ref), "05 Shear Force", "x [m]", "Force [N]");

        assert_abs_diff_eq!(x_num, x_ref, epsilon=1e-3);
        assert_abs_diff_eq!(y_num, y_ref, epsilon=1e-3);
        assert_abs_diff_eq!(φ_num, φ_ref, epsilon=1e-3);
        assert_abs_diff_eq!(N_num, N_ref, epsilon=1e-6);
        assert_abs_diff_eq!(M_num, M_ref, epsilon=1e-6);
        assert_abs_diff_eq!(Q_num, Q_ref, epsilon=1e-6);
    }
}

#[test]
fn nonlinear_straight_uniform_cantilever() {
    // A straight beam with uniform cross section is bent by a transversal and a normal force placed at the beam tip while the other end is clamped.
    // The resulting large deformations are compared to a FEM reference solution and the cross section forces to analytical results from static equilibrium.

    // Reference solution from Abaqus
    let u_ref = &[[0.0, 0.0], [0.099635508, 0.00738275], [0.19729113, 0.0285538], [0.29152731, 0.0618318], [0.3814916, 0.105388], [0.4668681, 0.157389], [0.5477956, 0.216094], [0.6247747, 0.279902], [0.698577, 0.347369], [0.770164, 0.417189], [0.840615, 0.488158]];
    let f_ref = DVector::<f64>::from_row_slice(&[8.3552, 52.361, 146.61, 287.30, 474.93, 709.47]);

    // Beam parameters
    let l = 1.0;
    let w0 = 0.01;
    let h0 = 0.01;
    let w1 = 0.01;
    let h1 = 0.01;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    let Fx = -200.0;
    let Fy = 200.0;

    // Beam bow
    let mut model = BowModel::default();
    model.settings.n_limb_elements = 25;
    model.settings.n_limb_eval_points = u_ref.len();
    model.settings.n_draw_steps = 5;
    model.materials = vec![Material::new("", "", rho, E, G)];
    model.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Line(LineInput::new(l))]);
    model.width = Width::new(vec![[0.0, w0], [1.0, w1]]);
    model.layers = vec![Layer::new("", 0, vec![[0.0, h0], [1.0, h1]])];

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_load(&model, Fx, Fy, 0.0).unwrap();

    let mut plotter = Plotter::new();

    // Check distance of node positions to reference solution
    for i in 0..setup.limb.length.len() {
        plotter.add_point((state.limb_pos[i][0], state.limb_pos[i][1]), (u_ref[i][0], u_ref[i][1]), "01 Bending Line", "x [m]", "y [m]");
        assert_abs_diff_eq!(f64::hypot(state.limb_pos[i][0] - u_ref[i][0], state.limb_pos[i][1] - u_ref[i][1]), 0.0, epsilon=0.5e-3);
    }

    // For analytical normal force, shear force and bending moments
    let x_end = state.limb_pos.last().unwrap()[0];
    let y_end = state.limb_pos.last().unwrap()[1];

    let F_max = f64::hypot(Fx, Fy);
    let M_max = F_max*l;

    // Check section forces and moments against analytical solution
    for i in 0..setup.limb.length.len() {
        let x = state.limb_pos[i][0];
        let y = state.limb_pos[i][1];
        let φ = state.limb_pos[i][2];

        let M_ref = Fy*(x_end - x) - Fx*(y_end - y);
        let N_ref = Fx*f64::cos(φ) + Fy*f64::sin(φ);
        let Q_ref = Fy*f64::cos(φ) - Fx*f64::sin(φ);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][0]), (setup.limb.length[i], N_ref), "02 Normal Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][0], N_ref, epsilon=1e-3*F_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][1]), (setup.limb.length[i], M_ref), "03 Bending Moment", "Length [m]", "Moment [Nm]");
        assert_abs_diff_eq!(state.limb_force[i][1], M_ref, epsilon=1e-3*M_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][2]), (setup.limb.length[i], Q_ref), "04 Shear Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][2], Q_ref, epsilon=1e-3*F_max);
    }

    // Compute natural frequencies
    let (_, modes) = Simulation::simulate_natural_frequencies(&model).unwrap();
    let f: Vec<f64> = modes.iter()
        .take(6)
        .map(|mode| mode.omega0/TAU)
        .collect();

    // Check natural frequencies against reference solution
    for i in 0..f.len() {
        plotter.add_point((i as f64 + 1.0, f[i]), (i as f64 + 1.0, f_ref[i]), "05 Natural Frequencies", "Order", "Frequency [Hz]");
        assert_relative_eq!(f[i], f_ref[i], max_relative=2e-2);
    }
}

#[test]
fn nonlinear_straight_tapered_cantilever() {
    // A straight beam with tapered cross section is bent by a transversal and a normal force placed at the beam tip while the other end is clamped.
    // The resulting large deformations are compared to a FEM reference solution and the cross section forces to analytical results from static equilibrium.

    // Reference solution from Abaqus
    let u_ref = &[[0.0, 0.0], [0.099899573, 0.0038425], [0.199126303, 0.0159811], [0.29677479, 0.0373441], [0.3916491, 0.068785], [0.4822534, 0.110957], [0.5668554, 0.164138], [0.6436886, 0.228025], [0.7113904, 0.301526], [0.769815, 0.382624], [0.821444, 0.468253]];
    let f_ref  = DVector::<f64>::from_row_slice(&[10.988, 46.438, 115.40, 218.11, 354.89, 525.78]);

    // Beam parameters
    let l = 1.0;
    let w0 = 0.01;
    let h0 = 0.01;
    let w1 = 0.005;
    let h1 = 0.005;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    let Fx = -100.0;
    let Fy = 100.0;

    // Beam bow
    let mut model = BowModel::default();
    model.settings.n_limb_elements = 25;
    model.settings.n_limb_eval_points = u_ref.len();
    model.settings.n_draw_steps = 5;
    model.materials = vec![Material::new("", "", rho, E, G)];
    model.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Line(LineInput::new(l))]);
    model.width = Width::new(vec![[0.0, w0], [1.0, w1]]);
    model.layers = vec![Layer::new("", 0, vec![[0.0, h0], [1.0, h1]])];

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_load(&model, Fx, Fy, 0.0).unwrap();

    let mut plotter = Plotter::new();

    // Check distance of node positions to reference solution
    for i in 0..setup.limb.length.len() {
        plotter.add_point((state.limb_pos[i][0], state.limb_pos[i][1]), (u_ref[i][0], u_ref[i][1]), "01 Bending Line", "x [m]", "y [m]");
        assert_abs_diff_eq!(f64::hypot(state.limb_pos[i][0] - u_ref[i][0], state.limb_pos[i][1] - u_ref[i][1]), 0.0, epsilon=0.5e-3);
    }

    // For analytical normal force, shear force and bending moments
    let x_end = state.limb_pos.last().unwrap()[0];
    let y_end = state.limb_pos.last().unwrap()[1];

    let F_max = f64::hypot(Fx, Fy);
    let M_max = F_max*l;

    // Check section forces and moments against analytical solution
    for i in 0..setup.limb.length.len() {
        let x = state.limb_pos[i][0];
        let y = state.limb_pos[i][1];
        let φ = state.limb_pos[i][2];

        let M_ref = Fy*(x_end - x) - Fx*(y_end - y);
        let N_ref = Fx*f64::cos(φ) + Fy*f64::sin(φ);
        let Q_ref = Fy*f64::cos(φ) - Fx*f64::sin(φ);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][0]), (setup.limb.length[i], N_ref), "02 Normal Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][0], N_ref, epsilon=1e-3*F_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][1]), (setup.limb.length[i], M_ref), "03 Bending Moment", "Length [m]", "Moment [Nm]");
        assert_abs_diff_eq!(state.limb_force[i][1], M_ref, epsilon=1e-3*M_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][2]), (setup.limb.length[i], Q_ref), "04 Shear Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][2], Q_ref, epsilon=1e-3*F_max);
    }

    // Compute natural frequencies
    let (_, modes) = Simulation::simulate_natural_frequencies(&model).unwrap();
    let f: Vec<f64> = modes.iter()
        .take(6)
        .map(|mode| mode.omega0/TAU)
        .collect();

    // Check natural frequencies against reference solution
    for i in 0..f.len() {
        plotter.add_point((i as f64 + 1.0, f[i]), (i as f64 + 1.0, f_ref[i]), "05 Natural Frequencies", "Order", "Frequency [Hz]");
        assert_relative_eq!(f[i], f_ref[i], max_relative=3e-2);
    }
}

#[test]
fn nonlinear_curved_uniform_cantilever() {
    // An initially curved beam with uniform cross section is bent by a transversal and a normal force placed at the beam tip while the other end is clamped.
    // The resulting large deformations are compared to a FEM reference solution and the cross section forces to analytical results from static equilibrium.

    // Reference solution from Abaqus
    let u_ref = &[[0.0000000000, 0.0000000000], [0.0991938223, 0.0109770126], [0.1938897170, 0.0425586916], [0.2805768688, 0.0921086100], [0.3568971572, 0.1565275789], [0.4215579792, 0.2326725894], [0.4741516572, 0.3176258035], [0.5149338347, 0.4088583701], [0.5446144270, 0.5042951151], [0.5641639374, 0.6023255993], [0.5747063162, 0.7017392286]];
    let f_ref  = DVector::<f64>::from_row_slice(&[8.4604, 49.038, 142.75, 283.14, 470.47, 704.52]);

    // Beam parameters
    let l = 1.0;
    let r = l/FRAC_PI_4;
    let w0 = 0.01;
    let h0 = 0.01;
    let w1 = 0.01;
    let h1 = 0.01;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    let Fx = -200.0;
    let Fy = 200.0;

    // Beam bow
    let mut model = BowModel::default();
    model.settings.n_limb_elements = 25;
    model.settings.n_limb_eval_points = u_ref.len();
    model.settings.n_draw_steps = 5;
    model.materials = vec![Material::new("", "", rho, E, G)];
    model.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Arc(ArcInput::new(l, r))]);
    model.width = Width::new(vec![[0.0, w0], [1.0, w1]]);
    model.layers = vec![Layer::new("", 0, vec![[0.0, h0], [1.0, h1]])];

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_load(&model, Fx, Fy, 0.0).unwrap();

    let mut plotter = Plotter::new();

    // Check distance of node positions to reference solution
    for i in 0..setup.limb.length.len() {
        plotter.add_point((state.limb_pos[i][0], state.limb_pos[i][1]), (u_ref[i][0], u_ref[i][1]), "01 Bending Line", "x [m]", "y [m]");
        //assert_abs_diff_eq!(f64::hypot(state.limb_pos[i][0] - u_ref[i][0], state.limb_pos[i][1] - u_ref[i][1]), 0.0, epsilon=0.5e-3);
    }

    // For analytical normal force, shear force and bending moments
    let x_end = state.limb_pos.last().unwrap()[0];
    let y_end = state.limb_pos.last().unwrap()[1];

    let F_max = f64::hypot(Fx, Fy);
    let M_max = F_max*l;

    // Check section forces and moments against analytical solution
    for i in 0..setup.limb.length.len() {
        let x = state.limb_pos[i][0];
        let y = state.limb_pos[i][1];
        let φ = state.limb_pos[i][2];

        let M_ref = Fy*(x_end - x) - Fx*(y_end - y);
        let N_ref = Fx*f64::cos(φ) + Fy*f64::sin(φ);
        let Q_ref = Fy*f64::cos(φ) - Fx*f64::sin(φ);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][0]), (setup.limb.length[i], N_ref), "02 Normal Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][0], N_ref, epsilon=1e-3*F_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][1]), (setup.limb.length[i], M_ref), "03 Bending Moment", "Length [m]", "Moment [Nm]");
        assert_abs_diff_eq!(state.limb_force[i][1], M_ref, epsilon=1e-3*M_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][2]), (setup.limb.length[i], Q_ref), "04 Shear Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][2], Q_ref, epsilon=1e-3*F_max);
    }

    // Compute natural frequencies
    let (_, modes) = Simulation::simulate_natural_frequencies(&model).unwrap();
    let f: Vec<f64> = modes.iter()
        .take(6)
        .map(|mode| mode.omega0/TAU)
        .collect();

    // Check natural frequencies against reference solution
    for i in 0..f.len() {
        plotter.add_point((i as f64 + 1.0, f[i]), (i as f64 + 1.0, f_ref[i]), "05 Natural Frequencies", "Order", "Frequency [Hz]");
        assert_relative_eq!(f[i], f_ref[i], max_relative=2e-2);
    }
}

#[test]
fn nonlinear_curved_tapered_cantilever() {
    // An initially curved beam with tapered cross section is bent by a transversal and a normal force placed at the beam tip while the other end is clamped.
    // The resulting large deformations are compared to a FEM reference solution and the cross section forces to analytical results from static equilibrium.

    // Reference solution from Abaqus
    let u_ref = &[[0.0000000000, 0.0000000000], [0.0996124043, 0.0075907526], [0.1967839770, 0.0307343916], [0.2887705288, 0.0696455100], [0.3725927572, 0.1239336789], [0.4452279792, 0.1924585894], [0.5039084572, 0.2732498035], [0.5465198347, 0.3635573701], [0.5720694270, 0.4601041151], [0.5811969374, 0.5595865993], [0.5767733162, 0.6594382286]];
    let f_ref  = DVector::<f64>::from_row_slice(&[11.110, 44.670, 113.23, 215.64, 352.29, 523.04]);

    // Beam parameters
    let l = 1.0;
    let r = l/FRAC_PI_4;
    let w0 = 0.01;
    let h0 = 0.01;
    let w1 = 0.005;
    let h1 = 0.005;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    let Fx = -100.0;
    let Fy = 100.0;

    // Beam bow
    let mut model = BowModel::default();
    model.settings.n_limb_elements = 25;
    model.settings.n_limb_eval_points = u_ref.len();
    model.settings.n_draw_steps = 5;
    model.materials = vec![Material::new("", "", rho, E, G)];
    model.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Arc(ArcInput::new(l, r))]);
    model.width = Width::new(vec![[0.0, w0], [1.0, w1]]);
    model.layers = vec![Layer::new("", 0, vec![[0.0, h0], [1.0, h1]])];

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_load(&model, Fx, Fy, 0.0).unwrap();

    let mut plotter = Plotter::new();

    // Check distance of node positions to reference solution
    for i in 0..setup.limb.length.len() {
        plotter.add_point((state.limb_pos[i][0], state.limb_pos[i][1]), (u_ref[i][0], u_ref[i][1]), "01 Bending Line", "x [m]", "y [m]");
        assert_abs_diff_eq!(f64::hypot(state.limb_pos[i][0] - u_ref[i][0], state.limb_pos[i][1] - u_ref[i][1]), 0.0, epsilon=0.5e-3);
    }

    // For analytical normal force, shear force and bending moments
    let x_end = state.limb_pos.last().unwrap()[0];
    let y_end = state.limb_pos.last().unwrap()[1];

    let F_max = f64::hypot(Fx, Fy);
    let M_max = F_max*l;

    // Check section forces and moments against analytical solution
    for i in 0..setup.limb.length.len() {
        let x = state.limb_pos[i][0];
        let y = state.limb_pos[i][1];
        let φ = state.limb_pos[i][2];

        let M_ref = Fy*(x_end - x) - Fx*(y_end - y);
        let N_ref = Fx*f64::cos(φ) + Fy*f64::sin(φ);
        let Q_ref = Fy*f64::cos(φ) - Fx*f64::sin(φ);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][0]), (setup.limb.length[i], N_ref), "02 Normal Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][0], N_ref, epsilon=1e-3*F_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][1]), (setup.limb.length[i], M_ref), "03 Bending Moment", "Length [m]", "Moment [Nm]");
        assert_abs_diff_eq!(state.limb_force[i][1], M_ref, epsilon=1e-3*M_max);

        plotter.add_point((setup.limb.length[i], state.limb_force[i][2]), (setup.limb.length[i], Q_ref), "04 Shear Force", "Length [m]", "Force [N]");
        assert_abs_diff_eq!(state.limb_force[i][2], Q_ref, epsilon=1e-3*F_max);
    }

    // Compute natural frequencies
    let (_, modes) = Simulation::simulate_natural_frequencies(&model).unwrap();
    let f: Vec<f64> = modes.iter()
        .take(6)
        .map(|mode| mode.omega0/TAU)
        .collect();

    // Check natural frequencies against reference solution
    for i in 0..f.len() {
        plotter.add_point((i as f64 + 1.0, f[i]), (i as f64 + 1.0, f_ref[i]), "05 Natural Frequencies", "Order", "Frequency [Hz]");
        assert_relative_eq!(f[i], f_ref[i], max_relative=5e-2);
    }
}

#[test]
fn nonlinear_straight_uniform_cantilever_offsets() {
    // Same bow as before, an initially straight beam with constant cross section, but solved for different offsets/alignments of the beam centerline.
    // The deflections in all cases must be geometrically consistent with each other and the natural frequencies must stay the same, since it is the same physical system.
    // TODO: Test passes, but precision isn't great

    // Beam parameters
    let l = 1.0;
    let w = 0.01;
    let h = 0.01;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    let Fx = -100.0;
    let Fy = 100.0;

    let solve_for_alignment = |alignment: LayerAlignment| {
        // Beam bow
        let mut model = BowModel::default();
        model.settings.n_limb_elements = 25;
        model.settings.n_limb_eval_points = 100;
        model.settings.n_draw_steps = 5;
        model.materials = vec![Material::new("", "", rho, E, G)];
        model.dimensions.handle_setback = match alignment {
            LayerAlignment::SectionBelly => -h/2.0,
            LayerAlignment::SectionCenter => 0.0,
            LayerAlignment::SectionBack => h/2.0,
            _ => unimplemented!()
        };
        model.profile = Profile::new(alignment, vec![SegmentInput::Line(LineInput::new(l))]);
        model.width = Width::new(vec![[0.0, w], [1.0, w]]);
        model.layers = vec![Layer::new("", 0, vec![[0.0, h], [1.0, h]])];

        // Compute static deflection
        let (_, state) = Simulation::simulate_static_load(&model, Fx, Fy, 0.0).unwrap();

        // Compute natural frequencies
        let (_, modes) = Simulation::simulate_natural_frequencies(&model).unwrap();
        let f: Vec<f64> = modes.iter()
            .take(6)
            .map(|mode| mode.omega0/TAU)
            .collect();

        (state.limb_pos, f)
    };

    let (pos0, f0) = solve_for_alignment(LayerAlignment::SectionCenter);
    let (pos1, f1) = solve_for_alignment(LayerAlignment::SectionBelly);
    let (pos2, f2) = solve_for_alignment(LayerAlignment::SectionBack);

    let mut plotter = Plotter::new();

    for i in 0..pos1.len() {
        // Position that the belly line should have: Position of the center line, offset by -h/2 in the direction of the cross section
        let x1_ref = pos0[i][0] + 0.5*h*f64::sin(pos0[i][2]);
        let y1_ref = pos0[i][1] - 0.5*h*f64::cos(pos0[i][2]);

        // Position that the back line should have: Position of the center line, offset by +h/2 in the direction of the cross section
        let x2_ref = pos0[i][0] - 0.5*h*f64::sin(pos0[i][2]);
        let y2_ref = pos0[i][1] + 0.5*h*f64::cos(pos0[i][2]);

        plotter.add_point((pos1[i][0], pos1[i][1]), (x1_ref, y1_ref), "01 Bending Line (Belly)", "x [m]", "y [m]");
        assert_abs_diff_eq!(f64::hypot(pos1[i][0] - x1_ref, pos1[i][1] - y1_ref), 0.0, epsilon=1e-3);

        plotter.add_point((pos2[i][0], pos2[i][1]), (x2_ref, y2_ref), "02 Bending Line (Back)", "x [m]", "y [m]");
        assert_abs_diff_eq!(f64::hypot(pos2[i][0] - x2_ref, pos2[i][1] - y2_ref), 0.0, epsilon=1e-3);
    }

    // Check natural frequencies against each other
    for i in 0..f1.len() {
        plotter.add_point((i as f64 + 1.0, f1[i]), (i as f64 + 1.0, f0[i]), "03 Natural Frequencies (Belly vs Center)", "Order", "Frequency [Hz]");
        assert_relative_eq!(f0[i], f1[i], max_relative=1e-2);

        plotter.add_point((i as f64 + 1.0, f2[i]), (i as f64 + 1.0, f0[i]), "04 Natural Frequencies (Back vs Center)", "Order", "Frequency [Hz]");
        assert_relative_eq!(f0[i], f2[i], max_relative=1e-2);

        plotter.add_point((i as f64 + 1.0, f1[i]), (i as f64 + 1.0, f2[i]), "05 Natural Frequencies (Belly vs Back)", "Order", "Frequency [Hz]");
        assert_relative_eq!(f1[i], f2[i], max_relative=1e-2);
    }
}

#[test]
fn nonlinear_curved_uniform_cantilever_offsets() {
    // Same bow as before, an initially curved beam with constant cross section, but solved for different offsets/alignments of the beam centerline.
    // The deflections in all cases must be geometrically consistent with each other and the natural frequencies must stay the same, since it is the same physical system.
    // TODO: Test passes, but precision isn't great

    // Beam parameters
    let l = 1.0;
    let r = l/FRAC_PI_4;
    let w = 0.01;
    let h = 0.01;

    let E = 210e9;
    let G = 80e9;
    let rho = 7850.0;

    let Fx = -100.0;
    let Fy = 100.0;

    let solve_for_alignment = |alignment: LayerAlignment| {
        // Offset
        let d = match alignment {
            LayerAlignment::SectionBelly => -h/2.0,
            LayerAlignment::SectionCenter => 0.0,
            LayerAlignment::SectionBack => h/2.0,
            _ => unimplemented!()
        };

        // Adjusted radius and length
        let rd = r - d;
        let ld = rd/r*l;

        // Beam bow
        let mut model = BowModel::default();
        model.settings.n_limb_elements = 20;
        model.settings.n_limb_eval_points = 100;
        model.settings.n_draw_steps = 5;
        model.materials = vec![Material::new("", "", rho, E, G)];
        model.dimensions.handle_setback = d;
        model.profile = Profile::new(alignment, vec![SegmentInput::Arc(ArcInput::new(ld, rd))]);
        model.width = Width::new(vec![[0.0, w], [1.0, w]]);
        model.layers = vec![Layer::new("", 0, vec![[0.0, h], [1.0, h]])];

        // Compute static deflection
        let (_, state) = Simulation::simulate_static_load(&model, Fx, Fy, 0.0).unwrap();

        // Compute natural frequencies
        let (_, modes) = Simulation::simulate_natural_frequencies(&model).unwrap();
        let f: Vec<f64> = modes.iter()
            .take(6)
            .map(|mode| mode.omega0/TAU)
            .collect();

        (state.limb_pos, f)
    };

    let (pos0, f0) = solve_for_alignment(LayerAlignment::SectionCenter);
    let (pos1, f1) = solve_for_alignment(LayerAlignment::SectionBelly);
    let (pos2, f2) = solve_for_alignment(LayerAlignment::SectionBack);

    let mut plotter = Plotter::new();

    for i in 0..pos1.len() {
        // Position that the belly line should have: Position of the center line, offset by -h/2 in the direction of the cross section
        let x1_ref = pos0[i][0] + 0.5*h*f64::sin(pos0[i][2]);
        let y1_ref = pos0[i][1] - 0.5*h*f64::cos(pos0[i][2]);

        // Position that the back line should have: Position of the center line, offset by +h/2 in the direction of the cross section
        let x2_ref = pos0[i][0] - 0.5*h*f64::sin(pos0[i][2]);
        let y2_ref = pos0[i][1] + 0.5*h*f64::cos(pos0[i][2]);

        plotter.add_point((pos1[i][0], pos1[i][1]), (x1_ref, y1_ref), "01 Bending Line (Belly)", "x [m]", "y [m]");
        assert_abs_diff_eq!(f64::hypot(pos1[i][0] - x1_ref, pos1[i][1] - y1_ref), 0.0, epsilon=1.7e-3);

        plotter.add_point((pos2[i][0], pos2[i][1]), (x2_ref, y2_ref), "02 Bending Line (Back)", "x [m]", "y [m]");
        assert_abs_diff_eq!(f64::hypot(pos2[i][0] - x2_ref, pos2[i][1] - y2_ref), 0.0, epsilon=1.7e-3);
    }

    // Check natural frequencies against each other
    for i in 0..f1.len() {
        plotter.add_point((i as f64 + 1.0, f1[i]), (i as f64 + 1.0, f0[i]), "03 Natural Frequencies (Belly vs Center)", "Order", "Frequency [Hz]");
        assert_relative_eq!(f0[i], f1[i], max_relative=2e-2);

        plotter.add_point((i as f64 + 1.0, f2[i]), (i as f64 + 1.0, f0[i]), "04 Natural Frequencies (Back vs Center)", "Order", "Frequency [Hz]");
        assert_relative_eq!(f0[i], f2[i], max_relative=2e-2);

        plotter.add_point((i as f64 + 1.0, f1[i]), (i as f64 + 1.0, f2[i]), "05 Natural Frequencies (Belly vs Back)", "Order", "Frequency [Hz]");
        assert_relative_eq!(f1[i], f2[i], max_relative=2e-2);
    }
}