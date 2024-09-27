use itertools::Itertools;
use crate::bow::input::BowInput;
use crate::bow::simulation::Simulation;
use crate::tests::utils::plotter::Plotter;

// Tests with the example bows

#[test]
fn bow_69w865k9() {
    perform_bow_test("bows/users/69w865k9.bow");
}

#[test]
fn bow_5g6c4m63() {
    perform_bow_test("bows/users/5g6c4m63.bow");
}

/*
// Needs string contact to brace
#[test]
fn bow_mt22m5gs() {
    perform_bow_test("bows/users/mt22m5gs.bow");
}
*/

/*
// TODO: Malformed geometry for some reason (bad conversion?)
#[test]
fn bow_hu78d41t() {
    perform_bow_test("bows/users/hu78d41t.bow");
}
*/

#[test]
fn bow_nv1x16ok() {
    perform_bow_test("bows/users/nv1x16ok.bow");
}

/*
// Recurve
#[test]
fn bow_d75f0aoh() {
    perform_bow_test("bows/users/d75f0aoh.bow");
}
*/

/*
// File does not load
#[test]
fn bow_j290alr2() {
    perform_bow_test("bows/users/j290alr2.bow");
}
*/

/*
// Recurve
#[test]
fn bow_cz183o6o() {
    perform_bow_test("bows/users/cz183o6o.bow");
}
*/

/*
// Recurve
#[test]
fn bow_2r77c5r2() {
    perform_bow_test("bows/users/2r77c5r2.bow");
}
*/

/*
// Dynamic simulation fails
#[test]
fn bow_w517u4bs() {
    perform_bow_test("bows/users/w517u4bs.bow");
}
*/

#[test]
fn bow_5y3n27ra() {
    perform_bow_test("bows/users/5y3n27ra.bow");
}

#[test]
fn bow_b282hcap() {
    perform_bow_test("bows/users/b282hcap.bow");
}

#[test]
fn bow_7b34ggm3() {
    perform_bow_test("bows/users/7b34ggm3.bow");
}

/*
// Rercurve
#[test]
fn bow_7j2rnu0n() {
    perform_bow_test("bows/users/7j2rnu0n.bow");
}
*/

/*
// File does not load
#[test]
fn bow_ibp08w9x() {
    perform_bow_test("bows/users/ibp08w9x.bow");
}
*/

#[test]
fn bow_36d2b7az() {
    perform_bow_test("bows/users/36d2b7az.bow");
}

#[test]
fn bow_834ozgt0() {
    perform_bow_test("bows/users/834ozgt0.bow");
}

#[test]
fn bow_v074x8zj() {
    perform_bow_test("bows/users/v074x8zj.bow");
}

#[test]
fn bow_7263zcsk() {
    perform_bow_test("bows/users/7263zcsk.bow");
}

#[test]
fn bow_c2h3p5y2() {
    perform_bow_test("bows/users/c2h3p5y2.bow");
}

/*
// File does not load
#[test]
fn bow_v3u2t11b() {
    perform_bow_test("bows/users/v3u2t11b.bow");
}
*/

fn perform_bow_test(file: &str) {
    // Load bow model from file
    let model = BowInput::load(file).expect("Failed to load bow file");

    // Perform modal analysis of the limb and check if the damping ratio of the first mode
    // is equal to the desired value defined in the model
    let (_, modes) = Simulation::simulate_limb_modes(&model).unwrap();
    assert_abs_diff_eq!(modes[0].zeta, model.damping.damping_ratio_limbs, epsilon=1e-5);  // TODO: Can this be made more accurate?

    let mut plotter = Plotter::new();
    for (i, mode) in modes.iter().enumerate() {
        plotter.add_point((i as f64, mode.omega), (i as f64, 0.0), "Modal Frequency", "Mode [-]", "Omega [1/s]");
        plotter.add_point((i as f64, mode.zeta), (i as f64, 0.0), "Modal Damping", "Mode [-]", "Zeta [-]");
    }

    let output = Simulation::simulate_dynamics(&model).unwrap();
    //output.save_json(outfile).expect("Failed to save simulation output");

    let statics = output.statics.unwrap();
    let states = statics.states;

    // Create a plot of the bending line in braced and fully drawn state

    for u in &states.limb_pos()[0] {
        plotter.add_point((u[0], u[1]), (0.0, 0.0), "Bending Line (braced)", "x [m]", "y [m]");
    }
    for u in &states.limb_pos()[states.len()-1] {
        plotter.add_point((u[0], u[1]), (0.0, 0.0), "Bending Line (drawn)", "x [m]", "y [m]");
    }

    // Check basic dimensions of the setup data
    assert_eq!(output.common.layers.len(), model.layers.len());
    for layer in &output.common.layers {
        assert_eq!(layer.length.len(), model.settings.n_layer_eval_points);
    }
    assert_eq!(output.common.limb.length.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.common.limb.width.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.common.limb.height.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.common.limb.position.len(), model.settings.n_limb_eval_points);

    // Basic dimension of the fixed static data
    assert_eq!(statics.min_layer_stresses.len(), model.layers.len());
    assert_eq!(statics.max_layer_stresses.len(), model.layers.len());

    // Check if number of states matches settings
    assert_eq!(states.len(), model.settings.n_draw_steps + 1);

    // Perform checks on each static state
    for (i, state) in states.iter().enumerate() {
        // Check basic dimensions
        assert_eq!(state.limb_pos.len(), model.settings.n_limb_eval_points);
        assert_eq!(state.limb_vel.len(), model.settings.n_limb_eval_points);
        assert_eq!(state.limb_acc.len(), model.settings.n_limb_eval_points);
        assert_eq!(state.string_pos.len(), 2);
        assert_eq!(state.string_vel.len(), 2);
        assert_eq!(state.string_acc.len(), 2);
        assert_eq!(state.limb_strain.len(), model.settings.n_limb_eval_points);
        assert_eq!(state.limb_force.len(), model.settings.n_limb_eval_points);
        assert_eq!(state.layer_strain.len(), model.layers.len());
        assert_eq!(state.layer_stress.len(), model.layers.len());
        assert!(state.layer_strain.iter().all(|layer| layer.len() == model.settings.n_layer_eval_points));
        assert!(state.layer_stress.iter().all(|layer| layer.len() == model.settings.n_layer_eval_points));

        // Check limb starting point (positions and angle)
        assert_abs_diff_eq!(state.limb_pos[0][0], 0.5*model.dimensions.handle_length, epsilon=1e-12);
        assert_abs_diff_eq!(state.limb_pos[0][1], model.dimensions.handle_setback, epsilon=1e-12);
        assert_abs_diff_eq!(state.limb_pos[0][2], model.dimensions.handle_angle, epsilon=1e-12);

        // Reference values for string force and grip force based on static considerations
        let string_pos_a = state.string_pos[1];
        let string_pos_b = state.string_pos[0];
        let alpha = f64::atan((string_pos_b[1] - string_pos_a[1])/(string_pos_b[0] - string_pos_a[0]));
        let draw_force_ref = 2.0*state.string_force*f64::sin(alpha);    // Reference draw force according to the string force
        let grip_force_ref = draw_force_ref;

        plotter.add_point((*state.draw_length, *state.string_force), (0.0, 0.0), "String Force", "Draw length [m]", "String force [N]");
        plotter.add_point((*state.draw_length, *state.draw_force), (*state.draw_length, draw_force_ref), "Draw Force", "Draw length [m]", "Draw force [N]");
        plotter.add_point((*state.draw_length, *state.grip_force), (*state.draw_length, grip_force_ref), "Grip Force", "Draw length [m]", "Grip force [N]");

        assert_abs_diff_eq!(*state.draw_force, draw_force_ref, epsilon=1e-3*statics.final_draw_force);
        assert_abs_diff_eq!(*state.grip_force, grip_force_ref, epsilon=1e-3*statics.final_draw_force);

        // Actual drawing work as elastic energy of limb and string compared to the initial (braced) state
        let drawing_work = *state.e_pot_limbs + *state.e_pot_string - (states.e_pot_limbs()[0] + states.e_pot_string()[0]);

        // Drawing work numerically approximated by integrating the force-draw curve
        let drawing_work_ref: f64 = states.iter().take(i+1).tuple_windows().map(|(prev, next)| {
            0.5*(prev.draw_force + next.draw_force)*(next.draw_length - prev.draw_length)     // Trapezoidal rule
        }).sum();

        //plotter.add_point((*state.draw_length, drawing_work), (*state.draw_length, drawing_work_ref), "Drawing Work", "Draw length [m]", "Drawing work [N]");
        assert_abs_diff_eq!(drawing_work, drawing_work_ref, epsilon=1e-3*states.e_pot_limbs()[0]);

        // Limb endpoint
        let x_end = state.limb_pos.last().unwrap()[0];
        let y_end = state.limb_pos.last().unwrap()[1];

        // Cartesian components of the string force
        let Fx = -state.string_force*f64::cos(alpha);
        let Fy = -state.string_force*f64::sin(alpha);

        for (j, &s) in output.common.limb.length.iter().enumerate() {
            let x = state.limb_pos[j][0];
            let y = state.limb_pos[j][1];
            let φ = state.limb_pos[j][2];

            // Reference values for cross section forces based on equilibrium with the string force
            let M_ref = Fy*(x_end - x) - Fx*(y_end - y);
            let N_ref = Fx*f64::cos(φ) + Fy*f64::sin(φ);
            let Q_ref = Fy*f64::cos(φ) - Fx*f64::sin(φ);

            //plotter.add_point((s, state.limb_force[j][0]), (s, N_ref), &format!("Normal Force {}", i), "Length [m]", "Normal force [N]");
            assert_abs_diff_eq!(state.limb_force[j][0], N_ref, epsilon=1e-3*statics.final_draw_force);

            //plotter.add_point((s, state.limb_force[j][1]), (s, M_ref), &format!("Bending Moment {}", i), "Length [m]", "Bending moment [Nm]");
            assert_abs_diff_eq!(state.limb_force[j][1], M_ref, epsilon=1e-3*statics.final_draw_force*model.dimensions.draw_length);

            //plotter.add_point((s, state.limb_force[j][2]), (s, Q_ref), &format!("Shear Force {}", i), "Length [m]", "Shear force [N]");
            assert_abs_diff_eq!(state.limb_force[j][2], Q_ref, epsilon=1e-3*statics.final_draw_force);
        }
    }

    let dynamics = output.dynamics.unwrap();
    let states = dynamics.states;

    // Perform checks on each dynamic state
    for (_, state) in states.iter().enumerate() {
        plotter.add_point((*state.time, *state.arrow_pos), (*state.time, 0.0), "Arrow Position", "Time [s]", "Position [m]");
        plotter.add_point((*state.time, *state.arrow_vel), (*state.time, 0.0), "Arrow Velocity", "Time [s]", "Velocity [m/s]");
        plotter.add_point((*state.time, *state.arrow_acc), (*state.time, 0.0), "Arrow Acceleration", "Time [s]", "Acceleration [m/s²]");
    }
}