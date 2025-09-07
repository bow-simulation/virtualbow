/*
// Various nonlinear beams are simulated (static/dynamic/eigen) and compared with
// reference results obtained with GXBeam (https://github.com/byuflowlab/GXBeam.jl)

use std::f64::consts::TAU;
use std::fs::File;
use approx::{assert_abs_diff_eq, assert_relative_eq};
use nalgebra::{vector, DMatrix, DVector, SMatrix, SVector};
use serde::Deserialize;
use virtualbow_num::fem::elements::beam::geometry::{CrossSection, PlanarCurve};
use virtualbow_num::utils::plotter::Plotter;

#[derive(Deserialize)]
struct Output {
    statics: Vec<OutputState>,
    dynamics: Vec<OutputState>,
    eigen: Vec<f64>
}

#[derive(Deserialize)]
struct OutputState {
    x: Vec<f64>,
    y: Vec<f64>
}

impl Output {
    pub fn load(path: &str) -> Self {
        let file = File::open(&path).expect("Failed to open file");
        serde_json::from_reader(file).expect("Failed to deserialize")
    }
}

struct Line {
    l: f64
}

impl Line {
    fn new(l: f64) -> Self {
        Self{l}
    }
}

impl PlanarCurve for Line {
    fn s_start(&self) -> f64 {
        0.0
    }

    fn s_end(&self) -> f64 {
        self.l
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        vector![s, 0.0]
    }

    fn angle(&self, s: f64) -> f64 {
        0.0
    }

    fn curvature(&self, s: f64) -> f64 {
        0.0
    }
}

struct Section {
    w: [f64; 4],
    h: [f64; 4],
    y: [f64; 4]
}

impl Section {
    fn constant(w: f64, h: f64, y: f64) -> Self {
        Self {
            w: [w; 4],
            h: [h; 4],
            y: [y; 4]
        }
    }

    fn linear(w: [f64; 2], h: [f64; 2], y: [f64; 2]) -> Self {
        Self {
            w: [w[0], w[0] + 1.0/3.0*(w[1] - w[0]), w[0] + 2.0/3.0*(w[1] - w[0]), w[1]],
            h: [h[0], h[0] + 1.0/3.0*(h[1] - h[0]), h[0] + 2.0/3.0*(h[1] - h[0]), h[1]],
            y: [y[0], w[0] + 1.0/3.0*(y[1] - y[0]), y[0] + 2.0/3.0*(y[1] - y[0]), y[1]],
        }
    }

    fn cubic(w: [f64; 4], h: [f64; 4], y: [f64; 4]) -> Self {
        Self {
            w,
            h,
            y
        }
    }
}

impl CrossSection for Section {
    fn ρA(&self, p: f64) -> f64 {
        todo!()
    }

    fn ρI(&self, p: f64) -> f64 {
        todo!()
    }

    fn stiffness(&self, p: f64) -> SMatrix<f64, 3, 3> {
        todo!()
    }

    fn width(&self, p: f64) -> f64 {
        todo!()
    }

    fn height(&self, p: f64) -> f64 {
        todo!()
    }

    fn strain_eval(&self, p: f64) -> DMatrix<f64> {
        todo!()
    }

    fn stress_eval(&self, p: f64) -> DMatrix<f64> {
        todo!()
    }
}


#[test]
fn example_beam() {
    let output = Output::load("data/gxbeam/output.json");

    let curve = Line::new(0.9);




    for i in 0..output.statics[10].x.len() {
        println!("{}, {}", output.statics[10].x[i], output.statics[10].y[i]);
    }
}
*/

/*
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
    let mut model = BowModel::example();
    model.damping = Default::default();    // Disable damping
    model.settings.num_limb_elements = 25;
    model.settings.num_limb_eval_points = u_ref.len();
    model.settings.min_draw_resolution = 5;
    model.dimensions.handle_reference = HandleReference::Profile;
    model.profile = Profile::new(vec![ProfileSegment::Line(Line::new(l))]);
    model.section = Section::new(LayerAlignment::SectionCenter, Width::linear(w0, w1), vec![Material::new("material", "#000000", rho, E, G)], vec![Layer::new("layer", "material", Height::linear(h0, h1))]);

    // Compute static deflection
    let (setup, state) = Simulation::simulate_static_limb(&model, Fx, Fy, 0.0).unwrap();

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
    let (_, modes) = Simulation::simulate_limb_modes(&model).unwrap();
    let f: Vec<f64> = modes.iter()
        .take(6)
        .map(|mode| mode.omega/TAU)
        .collect();

    // Check natural frequencies against reference solution
    for i in 0..f.len() {
        plotter.add_point((i as f64 + 1.0, f[i]), (i as f64 + 1.0, f_ref[i]), "05 Natural Frequencies", "Order", "Frequency [Hz]");
        assert_relative_eq!(f[i], f_ref[i], max_relative=2e-2);
    }
}
*/