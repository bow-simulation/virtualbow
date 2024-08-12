use itertools::Itertools;
use crate::bow::model::BowModel;
use crate::bow::simulation::Simulation;
use crate::tests::utils::plotter::Plotter;

// Tests with the example bows

#[test]
fn hybrid_longbow_including_riser_fade() {
    perform_bow_test("bows/users/003 hybrid longbow including riser fade.bow");
}

#[test]
fn hybrid_longbow_with_riser_block() {
    perform_bow_test("bows/users/003 hybrid longbow with riser block.bow");
}

/*
// Needs string contact to brace
#[test]
fn aktuell_170() {
    perform_bow_test("bows/users/aktuell-170.bow");
}
*/

#[test]
fn am_1896_444() {
    perform_bow_test("bows/users/Am.1896-.444.bow");
}

#[test]
fn big_paddle_ash() {
    perform_bow_test("bows/users/Big paddle ash.bow");
}

/*
// Recurve
#[test]
fn goguryeo_hwarang2013() {
    perform_bow_test("bows/users/Goguryeo_hwarang2013.bow");
}
*/

/*
// File does not load
#[test]
fn joshs_bow() {
    perform_bow_test("bows/users/Joshs bow.bow");
}
*/

/*
// Recurve
#[test]
fn komposiitti() {
    perform_bow_test("bows/users/komposiitti.bow");
}
*/

/*
// Recurve
#[test]
fn komposiitti_2() {
    perform_bow_test("bows/users/komposiitti 2.bow");
}
*/

#[test]
fn mollegabet1() {
    perform_bow_test("bows/users/mollegabet1.bow");
}

#[test]
fn paddlebow() {
    perform_bow_test("bows/users/paddlebow.bow");
}

#[test]
fn problem() {
    perform_bow_test("bows/users/Problem.bow");
}

#[test]
fn problematic_model() {
    perform_bow_test("bows/users/Problematic model.bow");
}

/*
// Rercurve
#[test]
fn ragim_black_bear() {
    perform_bow_test("bows/users/RAGIM_Black_Bear.bow");
}
*/

/*
// File does not load
#[test]
fn recurve() {
    perform_bow_test("bows/users/recurve.bow");
}
*/

#[test]
fn test() {
    perform_bow_test("bows/users/TEST.bow");
}

#[test]
fn test2() {
    perform_bow_test("bows/users/TEST2.bow");
}

#[test]
fn testbow() {
    perform_bow_test("bows/users/testbow.bow");
}

#[test]
fn u1() {
    perform_bow_test("bows/users/U1.bow");
}

#[test]
fn virtuaalimassatesti() {
    perform_bow_test("bows/users/virtuaalimassatesti.bow");
}

/*
// File does not load
#[test]
fn wooden_crossbow() {
    perform_bow_test("bows/users/Wooden crossbow.bow");
}
*/

fn perform_bow_test(file: &str) {
    let model = BowModel::load(file).expect("Failed to load bow file");
    let output = Simulation::simulate_statics(&model).unwrap();
    //output.save_json(outfile).expect("Failed to save simulation output");

    let statics = output.statics.unwrap();
    let states = statics.states;

    // Create a plot of the bending line in braced and fully drawn state
    let mut plotter = Plotter::new();
    for u in &states.limb_pos[0] {
        plotter.add_point((u[0], u[1]), (0.0, 0.0), "Bending Line (braced)", "x [m]", "y [m]");
    }
    for u in &states.limb_pos[states.len()-1] {
        plotter.add_point((u[0], u[1]), (0.0, 0.0), "Bending Line (drawn)", "x [m]", "y [m]");
    }

    // Check basic dimensions of the setup data
    assert_eq!(output.setup.limb.layers.len(), model.layers.len());
    for layer in &output.setup.limb.layers {
        assert_eq!(layer.length.len(), model.settings.n_layer_eval_points);
    }
    assert_eq!(output.setup.limb.length.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.setup.limb.width.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.setup.limb.height.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.setup.limb.position.len(), model.settings.n_limb_eval_points);

    // Check if number of states matches settings
    assert_eq!(states.len(), model.settings.n_draw_steps + 1);

    // Perform checks on each state
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
        for layer in state.layer_strain {
            assert_eq!(layer.len(), model.settings.n_layer_eval_points);
        }
        for layer in state.layer_stress {
            assert_eq!(layer.len(), model.settings.n_layer_eval_points);
        }

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
        let drawing_work = *state.e_pot_limbs + *state.e_pot_string - (states.e_pot_limbs[0] + states.e_pot_string[0]);

        // Drawing work numerically approximated by integrating the force-draw curve
        let drawing_work_ref: f64 = states.iter().take(i+1).tuple_windows().map(|(prev, next)| {
            0.5*(prev.draw_force + next.draw_force)*(next.draw_length - prev.draw_length)     // Trapezoidal rule
        }).sum();

        plotter.add_point((*state.draw_length, drawing_work), (*state.draw_length, drawing_work_ref), "Drawing Work", "Draw length [m]", "Drawing work [N]");
        assert_abs_diff_eq!(drawing_work, drawing_work_ref, epsilon=1e-3*states.e_pot_limbs[0]);

        // Limb endpoint
        let x_end = state.limb_pos.last().unwrap()[0];
        let y_end = state.limb_pos.last().unwrap()[1];

        // Cartesian components of the string force
        let Fx = -state.string_force*f64::cos(alpha);
        let Fy = -state.string_force*f64::sin(alpha);

        for (j, &s) in output.setup.limb.length.iter().enumerate() {
            let x = state.limb_pos[j][0];
            let y = state.limb_pos[j][1];
            let φ = state.limb_pos[j][2];

            // Reference values for cross section forces based on equilibrium with the string force
            let M_ref = Fy*(x_end - x) - Fx*(y_end - y);
            let N_ref = Fx*f64::cos(φ) + Fy*f64::sin(φ);
            let Q_ref = Fy*f64::cos(φ) - Fx*f64::sin(φ);

            plotter.add_point((s, state.limb_force[j][0]), (s, N_ref), &format!("Normal Force {}", i), "Length [m]", "Normal force [N]");
            assert_abs_diff_eq!(state.limb_force[j][0], N_ref, epsilon=1e-3*statics.final_draw_force);

            plotter.add_point((s, state.limb_force[j][1]), (s, M_ref), &format!("Bending Moment {}", i), "Length [m]", "Bending moment [Nm]");
            assert_abs_diff_eq!(state.limb_force[j][1], M_ref, epsilon=1e-3*statics.final_draw_force*model.dimensions.draw_length);

            plotter.add_point((s, state.limb_force[j][2]), (s, Q_ref), &format!("Shear Force {}", i), "Length [m]", "Shear force [N]");
            assert_abs_diff_eq!(state.limb_force[j][2], Q_ref, epsilon=1e-3*statics.final_draw_force);
        }
    }
}