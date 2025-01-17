use itertools::Itertools;
use nalgebra::{SVector, vector};
use num::Zero;
use crate::bow::input::BowInput;
use crate::bow::output::{BowOutput, Common, LayerInfo, LimbInfo, State, StateVec};
use crate::bow::simulation::Simulation;
use crate::numerics::integration::integrate_fixed;
use crate::tests::utils::plotter::Plotter;
use crate::utils::minmax::discrete_maximum_1d;

// Example bows are loaded, simulated, and the results are verified by various consistency checks.

// TODO: Checks
// - static special results
// - dynamic special results

#[test]
fn bow_69w865k9() {
    simulate_and_check_bow("bows/users/69w865k9.bow");
}

#[test]
fn bow_5g6c4m63() {
    simulate_and_check_bow("bows/users/5g6c4m63.bow");
}

#[test]
fn bow_mt22m5gs() {
    simulate_and_check_bow("bows/users/mt22m5gs.bow");
}

/*
// TODO: Malformed geometry for some reason (bad conversion?)
#[test]
fn bow_hu78d41t() {
    perform_bow_test("bows/users/hu78d41t.bow");
}
*/

#[test]
fn bow_nv1x16ok() {
    simulate_and_check_bow("bows/users/nv1x16ok.bow");
}

#[test]
fn bow_d75f0aoh() {
    simulate_and_check_bow("bows/users/d75f0aoh.bow");
}

/*
// File does not load
#[test]
fn bow_j290alr2() {
    perform_bow_test("bows/users/j290alr2.bow");
}
*/

// Recurve
#[test]
fn bow_cz183o6o() {
    simulate_and_check_bow("bows/users/cz183o6o.bow");
}

// Recurve
#[test]
fn bow_2r77c5r2() {
    simulate_and_check_bow("bows/users/2r77c5r2.bow");
}

/*
// Dynamic simulation fails
#[test]
fn bow_w517u4bs() {
    perform_bow_test("bows/users/w517u4bs.bow");
}
*/

#[test]
fn bow_5y3n27ra() {
    simulate_and_check_bow("bows/users/5y3n27ra.bow");
}

#[test]
fn bow_b282hcap() {
    simulate_and_check_bow("bows/users/b282hcap.bow");
}

#[test]
fn bow_7b34ggm3() {
    simulate_and_check_bow("bows/users/7b34ggm3.bow");
}

// Recurve
#[test]
fn bow_7j2rnu0n() {
    simulate_and_check_bow("bows/users/7j2rnu0n.bow");
}

/*
// File does not load
#[test]
fn bow_ibp08w9x() {
    perform_bow_test("bows/users/ibp08w9x.bow");
}
*/

#[test]
fn bow_36d2b7az() {
    simulate_and_check_bow("bows/users/36d2b7az.bow");
}

#[test]
fn bow_834ozgt0() {
    simulate_and_check_bow("bows/users/834ozgt0.bow");
}

#[test]
fn bow_v074x8zj() {
    simulate_and_check_bow("bows/users/v074x8zj.bow");
}

#[test]
fn bow_7263zcsk() {
    simulate_and_check_bow("bows/users/7263zcsk.bow");
}

#[test]
fn bow_c2h3p5y2() {
    simulate_and_check_bow("bows/users/c2h3p5y2.bow");
}

/*
// File does not load
#[test]
fn bow_v3u2t11b() {
    perform_bow_test("bows/users/v3u2t11b.bow");
}
*/


// TODO: Improvements
// - Use relative error tolerances where it makes sense (especially derivatives)

// Performs a static and dynamic simulation of the given .bow file and verifies various logical and physical properties of the output results.
fn simulate_and_check_bow(file: &str) {
    // Load bow model from file and run checks
    let model = BowInput::load(file).expect("Failed to load bow file");
    check_modal_properties(&model);
    check_output(&model);

    /*
    // Create a plot of the bending line in braced and fully drawn state

    for u in &states.limb_pos[0] {
        plotter.add_point((u[0], u[1]), (0.0, 0.0), "Bending Line (braced)", "x [m]", "y [m]");
    }
    for u in &states.limb_pos[states.len()-1] {
        plotter.add_point((u[0], u[1]), (0.0, 0.0), "Bending Line (drawn)", "x [m]", "y [m]");
    }

    // Create a plot of the stresses in braced and fully drawn state

    for (i, stresses) in states.layer_stress[0].iter().enumerate() {
        for (j, [stress_belly, stress_back]) in stresses.iter().enumerate() {
            plotter.add_point((output.common.limb.length[j], *stress_back), (0.0, 0.0), &format!("Stress Layer {i} Back (braced)"), "Length [m]", "Normal Stress [Pa]");
            plotter.add_point((output.common.limb.length[j], *stress_belly), (0.0, 0.0), &format!("Stress Layer {i} Belly (braced)"), "Length [m]", "Normal Stress [Pa]");
        }
    }
    for (i, stresses) in states.layer_stress[states.len()-1].iter().enumerate() {
        for (j, [stress_belly, stress_back]) in stresses.iter().enumerate() {
            plotter.add_point((output.common.limb.length[j], *stress_back), (0.0, 0.0), &format!("Stress Layer {i} Back (drawn)"), "Length [m]", "Normal Stress [Pa]");
            plotter.add_point((output.common.limb.length[j], *stress_belly), (0.0, 0.0), &format!("Stress Layer {i} Belly (drawn)"), "Length [m]", "Normal Stress [Pa]");
        }
    }

    // Check basic dimensions of the setup data
    assert_eq!(output.common.layers.len(), model.layers.len());
    assert_eq!(output.common.limb.length.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.common.limb.width.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.common.limb.height.len(), model.settings.n_limb_eval_points);
    assert_eq!(output.common.limb.position.len(), model.settings.n_limb_eval_points);

    // Basic dimension of the fixed static data
    assert_eq!(statics.min_layer_stresses.len(), model.layers.len());
    assert_eq!(statics.max_layer_stresses.len(), model.layers.len());

    // Check if number of static states matches settings
    assert_eq!(states.len(), model.settings.min_draw_resolution + 1);

    // Check if static states are sorted by strictly increasing draw length with no duplicates
    assert!(states.draw_length.windows(2).all(|x| x[0] < x[1]));

    // Perform checks on each static state
    for (i, state) in states.iter().enumerate() {
        // Check basic properties of the bow state
        check_bow_state(&model, &state.to_owned());

        // Reference values for string force and grip force based on static considerations
        let string_pos_a = state.string_pos[1];
        let string_pos_b = state.string_pos[0];
        let alpha = f64::atan((string_pos_b[1] - string_pos_a[1])/(string_pos_b[0] - string_pos_a[0]));
        let draw_force_ref = 2.0*state.string_force*f64::sin(alpha);    // Reference draw force according to the string force
        let grip_force_ref = draw_force_ref;

        // For the first state, the string angle alpha must be zero
        if i == 0 {
            assert_abs_diff_eq!(alpha, 0.0, epsilon=1e-6);
        }

        plotter.add_point((*state.draw_length, *state.string_force), (0.0, 0.0), "String Force", "Draw length [m]", "String force [N]");
        plotter.add_point((*state.draw_length, *state.draw_force), (*state.draw_length, draw_force_ref), "Draw Force", "Draw length [m]", "Draw force [N]");
        plotter.add_point((*state.draw_length, *state.draw_stiffness), (*state.draw_length, 0.0), "Draw Stiffness", "Draw length [m]", "Draw stiffness [N/m]");  // TODO: Reference value?
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
        assert_abs_diff_eq!(drawing_work, drawing_work_ref, epsilon=0.5e-2*states.e_pot_limbs[0]);

        // Check equilibrium of forces and moments if the string does not contact the limb.
        // TODO: Handle the case when it does, which is more complicated because of the contact forces.
        if state.limb_pos.len() == 2 {
            // Limb endpoint
            let x_end = state.limb_pos.last().unwrap()[0];
            let y_end = state.limb_pos.last().unwrap()[1];

            // Cartesian components of the string force
            let Fx = -state.string_force*f64::cos(alpha);
            let Fy = -state.string_force*f64::sin(alpha);

            for (j, &_s) in output.common.limb.length.iter().enumerate() {
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
    }

    let dynamics = output.dynamics.unwrap();
    let states = dynamics.states;

    // Check if dynamic states are sorted by strictly increasing time with no duplicates
    assert!(states.time.windows(2).all(|x| x[0] < x[1]));

    // Perform checks on each dynamic state
    for (_, state) in states.iter().enumerate() {
        // Check basic properties of the bow state
        check_bow_state(&model, &state.to_owned());

        plotter.add_point((*state.time, *state.arrow_pos), (*state.time, 0.0), "Arrow Position", "Time [s]", "Position [m]");
        plotter.add_point((*state.time, *state.arrow_vel), (*state.time, 0.0), "Arrow Velocity", "Time [s]", "Velocity [m/s]");
        plotter.add_point((*state.time, *state.arrow_acc), (*state.time, 0.0), "Arrow Acceleration", "Time [s]", "Acceleration [m/s²]");
        plotter.add_point((*state.time, *state.string_force), (0.0, 0.0), "Dynamic String Force", "Time [s]", "String force [N]");
    }
    */
}

// Performs modal analysis of the limb and check if the damping ratio of the first mode
// is equal to the desired value defined in the model
fn check_modal_properties(model: &BowInput) {
    let (_, modes) = Simulation::simulate_limb_modes(&model).unwrap();
    assert_abs_diff_eq!(modes[0].zeta, model.damping.damping_ratio_limbs, epsilon=1e-5);  // TODO: Can this be made more accurate?

    let mut plotter = Plotter::new();
    for (i, mode) in modes.iter().enumerate() {
        plotter.add_point((i as f64, mode.omega), (i as f64, 0.0), "Modal Frequency", "Mode [-]", "Omega [1/s]");
        plotter.add_point((i as f64, mode.zeta), (i as f64, 0.0), "Modal Damping", "Mode [-]", "Zeta [-]");
    }
}

// Performs a static and a dynamic analysis and checks the properties of the outputs
fn check_output(model: &BowInput) {
    let output = Simulation::simulate_dynamics(&model).unwrap();
    check_common_output(&model, &output);
    check_static_output(&model, &output);
    check_dynamic_output(&model, &output);
}

// Checks the properties of the common output, i.e. the outputs that are independent of the simulation mode
fn check_common_output(model: &BowInput, output: &BowOutput) {
    let Common { limb, layers, string_length, string_mass, limb_mass } = &output.common;
    let LimbInfo { length, position, width, height, bounds } = &limb;

    // There must be as many lengths as there are limb evaluation points defined in the model
    // Lengths must be sorted in strictly ascending order and start at zero
    assert_eq!(length.len(), model.settings.n_limb_eval_points);
    assert!(length.iter().tuple_windows().all(|(a, b)| a < b));
    assert_eq!(length[0], 0.0);

    // Number of positions, widths, heights and bounds must match number of evaluation points
    assert_eq!(position.len(), model.settings.n_limb_eval_points);
    assert_eq!(width.len(), model.settings.n_limb_eval_points);
    assert_eq!(height.len(), model.settings.n_limb_eval_points);
    assert_eq!(bounds.len(), model.settings.n_limb_eval_points);

    // Width and height must be strictly positive
    assert!(width.iter().all(|&w| w > 0.0));
    assert!(height.iter().all(|&h| h > 0.0));

    // Number of layer bounds must be consistent with the number of layers defined in the model
    assert!(bounds.iter().all(|b| b.len() == model.layers.len() + 1));

    // Number of layers must match the number of layers defined in the model
    // Layer info doesn't contain much currently, but the layer names must not be empty
    assert_eq!(layers.len(), model.layers.len());
    for layer in layers {
        let LayerInfo { name } = layer;
        assert!(!name.is_empty());
    }

    // String length, string mass and limb mass must be positive
    assert!(*string_length > 0.0);
    assert!(*string_mass > 0.0);
    assert!(*limb_mass > 0.0);
}

fn check_static_output(model: &BowInput, output: &BowOutput) {
    check_general_state_properties(&model, &output.statics.as_ref().unwrap().states);
    check_static_state_properties(&model, &output);
    check_static_equilibrium(&model, &output);
    check_draw_derivatives(&model, &output);
}

fn check_dynamic_output(model: &BowInput, output: &BowOutput) {
    check_general_state_properties(&model, &output.dynamics.as_ref().unwrap().states);
    check_dynamic_state_properties(&model, &output);
    check_time_derivatives(&model, &output);
}

// Check basic properties for a series of static or dynamic bow states
fn check_general_state_properties(model: &BowInput, states: &StateVec) {
    for state in states.iter() {
        let State {
            time,
            draw_length,
            limb_pos,
            limb_vel,
            string_pos,
            string_vel,
            limb_strain,
            limb_force,
            layer_strain,
            layer_stress,
            arrow_pos: _,
            arrow_vel: _,
            arrow_acc: _,
            e_pot_limbs,
            e_kin_limbs,
            e_pot_string,
            e_kin_string,
            e_kin_arrow,
            draw_force: _,
            draw_stiffness: _,
            grip_force: _,
            string_force: _,
            strand_force: _
        } = state.to_owned();

        // Time must be non-negative
        assert!(time >= 0.0);

        // Draw length must not exceed dimension setting
        assert!(draw_length <= model.dimensions.draw_length);

        // Limb position and velocity must have as many entries as there are eval points
        assert_eq!(limb_pos.len(), model.settings.n_limb_eval_points);
        assert_eq!(limb_vel.len(), model.settings.n_limb_eval_points);

        // Limb starting point (positions and angle) must match the dimension settings
        assert_abs_diff_eq!(limb_pos[0][0], 0.5*model.dimensions.handle_length, epsilon=1e-12);
        assert_abs_diff_eq!(limb_pos[0][1], model.dimensions.handle_setback, epsilon=1e-12);
        assert_abs_diff_eq!(limb_pos[0][2], model.dimensions.handle_angle, epsilon=1e-12);

        // String position and velocity must have at least 2 entries and up to the maximum number of contact points defined by the number of limb elements
        // The position of the first string point must be consistent with the draw length
        assert!(string_pos.len() >= 2 && string_pos.len() <= model.settings.n_limb_elements + 2);
        assert!(string_vel.len() >= 2 && string_vel.len() <= model.settings.n_limb_elements + 2);
        assert_eq!(string_pos[0][1], -draw_length);

        // Limb strains and forces must have as many entries as there are eval points
        assert_eq!(limb_strain.len(), model.settings.n_limb_eval_points);
        assert_eq!(limb_force.len(), model.settings.n_limb_eval_points);

        // Layer stresses and strains must contain one vector for each layer.
        // Each of those vectors must contain as many entries as there are eval points.
        assert_eq!(layer_strain.len(), model.layers.len());
        assert_eq!(layer_stress.len(), model.layers.len());
        assert!(layer_strain.iter().all(|x| x.len() == model.settings.n_limb_eval_points));
        assert!(layer_stress.iter().all(|x| x.len() == model.settings.n_limb_eval_points));

        // All energies must be positive
        assert!(e_pot_limbs >= 0.0);
        assert!(e_kin_limbs >= 0.0);
        assert!(e_pot_string >= 0.0);
        assert!(e_kin_string >= 0.0);
        assert!(e_kin_arrow >= 0.0);
    }
}

// Check basic properties that are specific to a series of static bow states
fn check_static_state_properties(model: &BowInput, output: &BowOutput) {
    let statics = output.statics.as_ref().unwrap();
    let states = &statics.states;

    let ABS_TOL_FORCE = 1e-3*statics.final_draw_force;

    // First draw length must be equal to specified brace height, last draw length to specified draw length
    // The number of states must currently equal the minimum draw resolution (+1 because steps vs. points) since step size control isn't implemented yet
    // The states must be ordered by strictly increasing draw length
    assert_eq!(*states.draw_length.first().unwrap(), model.dimensions.brace_height);
    assert_eq!(*states.draw_length.last().unwrap(), model.dimensions.draw_length);
    assert_eq!(states.draw_length.len(), model.settings.min_draw_resolution + 1);
    assert!(states.draw_length.iter().tuple_windows().all(|(a, b)| a < b));

    // Initial draw force must be zero
    assert_abs_diff_eq!(*states.draw_force.first().unwrap(), 0.0, epsilon=ABS_TOL_FORCE);

    for state in states.iter() {
        let State {
            time,
            draw_length,
            limb_pos: _,
            limb_vel,
            string_pos: _,
            string_vel,
            limb_strain: _,
            limb_force: _,
            layer_strain: _,
            layer_stress: _,
            arrow_pos,
            arrow_vel,
            arrow_acc,
            e_pot_limbs: _,
            e_kin_limbs,
            e_pot_string: _,
            e_kin_string,
            e_kin_arrow,
            draw_force,
            draw_stiffness: _,
            grip_force,
            string_force,
            strand_force
        } = state.to_owned();

        // Time must be zero
        assert_eq!(time, 0.0);

        // Draw length must range from brace height to full draw
        assert!(draw_length >= model.dimensions.brace_height);
        assert!(draw_length <= model.dimensions.draw_length);

        // Limb and string velocities must be zero
        assert!(limb_vel.iter().all(SVector::is_zero));
        assert!(string_vel.iter().all(SVector::is_zero));

        // Arrow position must be identical to negative draw length, velocity and acceleration must be zero
        assert_eq!(arrow_pos, -draw_length);
        assert_eq!(arrow_vel, 0.0);
        assert_eq!(arrow_acc, 0.0);

        // Kinetic energies must be zero
        assert_eq!(e_kin_limbs, 0.0);
        assert_eq!(e_kin_string, 0.0);
        assert_eq!(e_kin_arrow, 0.0);

        // Draw force, string force and strand force must be positive
        assert!(draw_force >= -ABS_TOL_FORCE);  // Allow slightly negative draw force because static equilibrium at the braced state is not 100% accurate
        assert!(string_force >= 0.0);
        assert!(strand_force >= 0.0);

        // Grip force must be identical to negative draw force
        assert_abs_diff_eq!(grip_force, draw_force, epsilon=ABS_TOL_FORCE);
    }
}

// Check basic properties that are specific to a series of static bow states
fn check_dynamic_state_properties(model: &BowInput, output: &BowOutput) {
    let dynamics = output.dynamics.as_ref().unwrap();
    let states = &dynamics.states;

    let ABS_TOL_TIMESTEP = 1e-12;
    let ABS_TOL_ARROW_ACC = 1e-9*states.arrow_acc[0];

    // Time of the first state must be zero.
    // Timesteps between states must lie between the minimum and maximum defined in the settings,
    // except for the last step before arrow separation or the end of the simulation, which are allowed to be shorter.
    assert_eq!(states.time[0], 0.0);
    for (index, (t0, t1)) in states.time.iter().tuple_windows().enumerate() {
        if index != states.time.len() - 2 && dynamics.arrow_departure.as_ref().map(|x| x.state_idx) != Some(index + 1) {
            assert!(t1 - t0 >= model.settings.min_timestep - ABS_TOL_TIMESTEP);
        }
        assert!(t1 - t0 <= model.settings.max_timestep + ABS_TOL_TIMESTEP);
    }

    // Limb and string velocities must be zero initially
    assert!(states.limb_vel[0].iter().all(SVector::is_zero));
    assert!(states.string_vel[0].iter().all(SVector::is_zero));

    // Initial arrow position must be consistent with specified draw length, velocity must be zero
    assert_eq!(states.arrow_pos[0], -model.dimensions.draw_length);
    assert_eq!(states.arrow_vel[0], 0.0);

    for state in states.iter() {
        let State {
            time: _,
            draw_length: _,
            limb_pos: _,
            limb_vel: _,
            string_pos: _,
            string_vel: _,
            limb_strain: _,
            limb_force: _,
            layer_strain: _,
            layer_stress: _,
            arrow_pos,
            arrow_vel: _,
            arrow_acc,
            e_pot_limbs: _,
            e_kin_limbs: _,
            e_pot_string: _,
            e_kin_string: _,
            e_kin_arrow: _,
            draw_force,
            draw_stiffness,
            grip_force: _,
            string_force: _,
            strand_force: _
        } = state.to_owned();

        // Arrow position must not exceed draw length
        assert!(arrow_pos >= -model.dimensions.draw_length);

        // Arrow acceleration must be larger than critical acceleration due to clamp force
        assert!(arrow_acc >= -model.settings.arrow_clamp_force/model.masses.arrow - ABS_TOL_ARROW_ACC);

        // Draw force and draw stiffness must be zero
        assert_eq!(draw_force, 0.0);
        assert_eq!(draw_stiffness, 0.0);

        // Can't think of much else to check here...
    }
}

// Checks if the section forces and stresses are in equilibrium with the external forces of the bow
fn check_static_equilibrium(model: &BowInput, output: &BowOutput) {
    let statics = output.statics.as_ref().unwrap();
    let states = &statics.states;

    let ABS_TOL_ALPHA = 1e-6;                                                           // Tolerance for the string angle in braced state
    let ABS_TOL_FORCE = 1e-3*statics.final_draw_force;                                  // Tolerance for force comparisons
    let ABS_TOL_MOMENT = 1e-3*statics.final_draw_force*model.dimensions.draw_length;    // Tolerance for moment comparisons
    let ABS_TOL_ENERGY = 0.5e-2*states.e_pot_limbs[0];                                  // Tolerance for energy comparisons
    let REL_TOL_STRESS = 1e-6;

    // Perform checks on each static state
    // i: State
    // j: Length along limb
    // k: Cross section layer
    for (i, state) in states.iter().enumerate() {
        // Analytical values for the draw force and grip force according to the string force, string angle and static considerations
        let string_pos_a = state.string_pos[1];
        let string_pos_b = state.string_pos[0];
        let alpha = f64::atan((string_pos_b[1] - string_pos_a[1])/(string_pos_b[0] - string_pos_a[0]));
        let draw_force_ref = 2.0*state.string_force*f64::sin(alpha);    // Reference draw force according to the string force
        let grip_force_ref = draw_force_ref;                            // Grip force is equal to string force in the static case

        // Compare analytical string and grip forces to reference values
        assert_abs_diff_eq!(*state.draw_force, draw_force_ref, epsilon=ABS_TOL_FORCE);
        assert_abs_diff_eq!(*state.grip_force, grip_force_ref, epsilon=ABS_TOL_FORCE);

        // For the first state, the string angle alpha must be zero
        if i == 0 {
            assert_abs_diff_eq!(alpha, 0.0, epsilon=ABS_TOL_ALPHA);
        }

        // Actual drawing work as elastic energy of limb and string compared to the initial (braced) state
        let drawing_work = *state.e_pot_limbs + *state.e_pot_string - (states.e_pot_limbs[0] + states.e_pot_string[0]);

        // Drawing work numerically approximated by integrating the force-draw curve
        let drawing_work_ref: f64 = states.iter().take(i + 1).tuple_windows().map(|(prev, next)| {
            0.5*(prev.draw_force + next.draw_force)*(next.draw_length - prev.draw_length)     // Trapezoidal rule
        }).sum();

        // Compare actual and numerical drawing work
        assert_abs_diff_eq!(drawing_work, drawing_work_ref, epsilon=ABS_TOL_ENERGY);

        // Check equilibrium of the limb's cross section forces with the string force.
        // For now only if the string does not contact the limb, since that is more complicated.
        // TODO: Handle the case when it does, which is more complicated because of the contact forces.
        if state.string_pos.len() == 2 {
            for (j, &_s) in output.common.limb.length.iter().enumerate() {
                // Cross section forces according to the simulation output
                let N_out = state.limb_force[j][0];
                let M_out = state.limb_force[j][1];
                let Q_out = state.limb_force[j][2];

                // Calculate the cross section's normal force and bending moment by integrating the normal stresses over the cross section.
                // Compare the integrated forces to those from the simulation output to check if the stresses are consistent with the section forces.
                // (Shear force is not calculated since the shear stress is not yet part of the output results)

                let bounds = &output.common.limb.bounds[j];
                let width = output.common.limb.width[j];
                let mut forces = SVector::zeros();

                for (k, (&ya, &yb)) in bounds.iter().tuple_windows().enumerate() {
                    // Function of normal stress sigma and its moment sigma*y over the layer's height coordinate y
                    let stresses = |y: f64| {
                        let sigma_a = state.layer_stress[k][j][0];
                        let sigma_b = state.layer_stress[k][j][1];
                        let sigma_y = sigma_a + (y - ya)/(yb - ya)*(sigma_b - sigma_a);
                        width*vector![sigma_y, -y*sigma_y]
                    };

                    if ya != yb {
                        forces += integrate_fixed(stresses, ya, yb, 100);
                    }
                }

                // Cross section forces according to integration of the stresses
                let N_int = forces[0];
                let M_int = forces[1];

                assert_relative_eq!(N_int, N_out, max_relative=REL_TOL_STRESS);
                assert_relative_eq!(M_int, M_out, max_relative=REL_TOL_STRESS);

                // The next checks verify that the cross section forces are in balance with the external force
                // that the string exerts on the bow limb.

                // Limb endpoint
                let x_contact = string_pos_a[0];
                let y_contact = string_pos_a[1];

                // Cartesian components of the string force
                let Fx = -state.string_force*f64::cos(alpha);
                let Fy = -state.string_force*f64::sin(alpha);

                // Current position on the profile curve
                let x = state.limb_pos[j][0];
                let y = state.limb_pos[j][1];
                let φ = state.limb_pos[j][2];

                // Cross section according to static equilibrium with the string force
                let M_ref = Fy*(x_contact - x) - Fx*(y_contact - y);
                let N_ref = Fx*f64::cos(φ) + Fy*f64::sin(φ);
                let Q_ref = Fy*f64::cos(φ) - Fx*f64::sin(φ);

                assert_abs_diff_eq!(N_out, N_ref, epsilon=ABS_TOL_FORCE);
                assert_abs_diff_eq!(M_out, M_ref, epsilon=ABS_TOL_MOMENT);
                assert_abs_diff_eq!(Q_out, Q_ref, epsilon=ABS_TOL_FORCE);
            }
        }
    }
}

// Checks the time derivatives in a series of dynamic bow states, i.e. velocities and accelerations,
// by comparing them to finite difference approximations from the original data.
fn check_time_derivatives(_model: &BowInput, output: &BowOutput) {
    let dynamics = output.dynamics.as_ref().unwrap();
    let states = &dynamics.states;

    let V_MAX_ARROW = discrete_maximum_1d(&states.arrow_vel).0;          // Maximum arrow velocity as reference for comparison
    let A_MAX_ARROW = discrete_maximum_1d(&states.arrow_acc).0;          // Maximum arrow acceleration as reference for comparison
    let W_MAX_STRING = V_MAX_ARROW/(output.common.string_length/2.0);    // Estimated maximum angular velocity of the string

    let ABS_TOL_VEL_LINEAR = 1e-3*V_MAX_ARROW;    // Tolerance for linear velocity
    let ABS_TOL_ACC_LINEAR = 1e-3*A_MAX_ARROW;    // Tolerance for linear acceleration
    let ABS_TOL_VEL_ANGULAR = 1e-3*W_MAX_STRING;  // Tolerance for angular velocity

    for (index, (state0, state1)) in states.iter().tuple_windows().enumerate() {
        let State {
            time: time0,
            draw_length: _,
            limb_pos: limb_pos0,
            limb_vel: limb_vel0,
            string_pos: string_pos0,
            string_vel: string_vel0,
            limb_strain: _,
            limb_force: _,
            layer_strain: _,
            layer_stress: _,
            arrow_pos: arrow_pos0,
            arrow_vel: arrow_vel0,
            arrow_acc: arrow_acc0,
            e_pot_limbs: _,
            e_kin_limbs: _,
            e_pot_string: _,
            e_kin_string: _,
            e_kin_arrow: _,
            draw_force: _,
            draw_stiffness: _,
            grip_force: _,
            string_force: _,
            strand_force: _
        } = state0.to_owned();

        let State {
            time: time1,
            draw_length: _,
            limb_pos: limb_pos1,
            limb_vel: limb_vel1,
            string_pos: string_pos1,
            string_vel: string_vel1,
            limb_strain: _,
            limb_force: _,
            layer_strain: _,
            layer_stress: _,
            arrow_pos: arrow_pos1,
            arrow_vel: arrow_vel1,
            arrow_acc: arrow_acc1,
            e_pot_limbs: _,
            e_kin_limbs: _,
            e_pot_string: _,
            e_kin_string: _,
            e_kin_arrow: _,
            draw_force: _,
            draw_stiffness: _,
            grip_force: _,
            string_force: _,
            strand_force: _
        } = state1.to_owned();

        // Compare numerical arrow velocity (forward difference) to average velocity in the interval
        let arrow_vel_num = (arrow_pos1 - arrow_pos0)/(time1 - time0);
        let arrow_vel_avg = 0.5*(arrow_vel0 + arrow_vel1);
        assert_abs_diff_eq!(arrow_vel_avg, arrow_vel_num, epsilon=ABS_TOL_VEL_LINEAR);

        // Compare numerical arrow acceleration (forward difference) to average acceleration in the interval.
        // Skip this check if the separation of the arrow from the string occured between the states,
        // since this produces a jump in acceleration that we can't replicate with finite differences.
        if dynamics.arrow_departure.as_ref().map(|x| x.state_idx) != Some(index) {
            let arrow_acc_num = (arrow_vel1 - arrow_vel0)/(time1 - time0);
            let arrow_acc_avg = 0.5*(arrow_acc0 + arrow_acc1);
            assert_abs_diff_eq!(arrow_acc_avg, arrow_acc_num, epsilon=ABS_TOL_ACC_LINEAR);
        }

        // Compare numerical limb velocity (forward difference) to average velocity in the interval for each point of the limb.
        for i in 0..limb_pos0.len() {
            let limb_vel_num = (limb_pos1[i] - limb_pos0[i])/(time1 - time0);
            let limb_vel_avg = 0.5*(limb_vel0[i] + limb_vel1[i]);
            assert_abs_diff_eq!(limb_vel_avg[0], limb_vel_num[0], epsilon=ABS_TOL_VEL_LINEAR);
            assert_abs_diff_eq!(limb_vel_avg[1], limb_vel_num[1], epsilon=ABS_TOL_VEL_LINEAR);
            assert_abs_diff_eq!(limb_vel_avg[2], limb_vel_num[2], epsilon=ABS_TOL_VEL_ANGULAR);
        }

        // Compare numerical string velocity (forward difference) to average velocity in the interval for each point of the string.
        // Only do this if the number of contact points has not changed from one state to the next, otherwise we can't differentiate.
        // There will also be a problem if one contact appears and another disappears in the same timestep (=> same number of contacts), but the probability of this happening is low.
        if string_pos0.len() == string_pos1.len() {
            for i in 0..string_pos0.len() {
                let string_vel_num = (string_pos1[i] - string_pos0[i])/(time1 - time0);
                let string_vel_avg = 0.5*(string_vel0[i] + string_vel1[i]);
                assert_abs_diff_eq!(string_vel_avg, string_vel_num, epsilon=ABS_TOL_VEL_LINEAR);
            }
        }
    }
}

// Checks the derivatives wrt. draw length in a series of static bow states, i.e. energy, force and stiffness,
// by comparing them to finite difference approximations from the original data.
fn check_draw_derivatives(_model: &BowInput, output: &BowOutput) {
    let statics = output.statics.as_ref().unwrap();
    let states = &statics.states;

    let K_DRAW_MAX = discrete_maximum_1d(&states.draw_stiffness).0;          // Maximum draw stiffness as reference for comparison
    let ABS_TOL_DRAW_STIFFNESS = 1e-3*K_DRAW_MAX;
    let ABS_TOL_DRAW_FORCE = 1e-3*statics.final_draw_force;

    for (state0, state1) in states.iter().tuple_windows() {
        let State {
            time: _,
            draw_length: draw_length0,
            limb_pos: _,
            limb_vel: _,
            string_pos: string_pos0,
            string_vel: _,
            limb_strain: _,
            limb_force: _,
            layer_strain: _,
            layer_stress: _,
            arrow_pos: _,
            arrow_vel: _,
            arrow_acc: _,
            e_pot_limbs: e_pot_limbs0,
            e_kin_limbs: _,
            e_pot_string: e_pot_string0,
            e_kin_string: _,
            e_kin_arrow: _,
            draw_force: draw_force0,
            draw_stiffness: draw_stiffness0,
            grip_force: _,
            string_force: _,
            strand_force: _
        } = state0.to_owned();

        let State {
            time: _,
            draw_length: draw_length1,
            limb_pos: _,
            limb_vel: _,
            string_pos: string_pos1,
            string_vel: _,
            limb_strain: _,
            limb_force: _,
            layer_strain: _,
            layer_stress: _,
            arrow_pos: _,
            arrow_vel: _,
            arrow_acc: _,
            e_pot_limbs: e_pot_limbs1,
            e_kin_limbs: _,
            e_pot_string: e_pot_string1,
            e_kin_string: _,
            e_kin_arrow: _,
            draw_force: draw_force1,
            draw_stiffness: draw_stiffness1,
            grip_force: _,
            string_force: _,
            strand_force: _
        } = state1.to_owned();

        // Only consider states where the number of string contact points has not changed from one state to the next, otherwise we can't differentiate.
        if string_pos0.len() == string_pos1.len() {
            // Compare numerical draw force by differentiation of the total energy to the average draw force in the interval
            let draw_force_num = ((e_pot_limbs1 + e_pot_string1) - (e_pot_limbs0 + e_pot_string0))/(draw_length1 - draw_length0);
            let draw_force_avg = 0.5*(draw_force0 + draw_force1);
            assert_abs_diff_eq!(draw_force_avg, draw_force_num, epsilon=ABS_TOL_DRAW_FORCE);

            // Compare numerical draw stiffness (forward difference) to average stiffness in the interval
            let draw_stiffness_num = (draw_force1 - draw_force0)/(draw_length1 - draw_length0);
            let draw_stiffness_avg = 0.5*(draw_stiffness0 + draw_stiffness1);
            assert_abs_diff_eq!(draw_stiffness_avg, draw_stiffness_num, epsilon=ABS_TOL_DRAW_STIFFNESS);
        }
    }
}