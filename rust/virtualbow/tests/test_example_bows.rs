use std::f64::consts::FRAC_PI_2;
use std::path::Path;
use virtualbow::input::BowModel;
use virtualbow::output::{ArrowDeparture, BowResult, Common, Dynamics, LayerInfo, LimbInfo, MaxForces, MaxStresses, State, StateVec, Statics};
use virtualbow::simulation::Simulation;
use virtualbow_num::utils::integration::fixed_simpson;
use virtualbow_num::utils::minmax::discrete_maximum_1d;
use virtualbow_num::testutils::plotter::Plotter;

use itertools::Itertools;
use nalgebra::{SVector, vector};
use assert2::assert;
use approx::{assert_abs_diff_eq, assert_relative_eq};
use num::Zero;
use test_each_file::test_each_path;

// Example bows are loaded, simulated, and the results are verified by various consistency checks
// TODO: Improvements
// - Use relative error tolerances where it makes sense (especially derivatives)

// Generate test cases from files
test_each_path!{ in "virtualbow/data/examples" => simulate_and_check_bow }

// Performs a static and dynamic simulation of the given .bow file and verifies various logical and physical properties of the output results.
fn simulate_and_check_bow(file: &Path) {
    let mut plotter = Plotter::new();

    // Load bow model from file and run checks
    let model = BowModel::load(file).expect("Failed to load bow file");
    check_modal_properties(&mut plotter, &model);
    check_output(&mut plotter, &model);
}

// Performs modal analysis of the limb and check if the damping ratio of the first mode
// is equal to the desired value defined in the model
fn check_modal_properties(plotter: &mut Plotter, model: &BowModel) {
    let (_, modes) = Simulation::simulate_limb_modes(model).unwrap();
    assert_abs_diff_eq!(modes[0].zeta, model.damping.damping_ratio_limbs, epsilon=1e-5);  // TODO: Can this be made more accurate?

    for (i, mode) in modes.iter().enumerate() {
        plotter.add_point((i as f64, mode.omega), (i as f64, 0.0), "Modal Frequency", "Mode [-]", "Omega [1/s]");
        plotter.add_point((i as f64, mode.zeta), (i as f64, 0.0), "Modal Damping", "Mode [-]", "Zeta [-]");
    }
}

// Performs a static and a dynamic analysis and checks the properties of the outputs
fn check_output(plotter: &mut Plotter, model: &BowModel) {
    let output = Simulation::simulate_dynamics(model).unwrap();
    check_common_output(model, &output);
    check_static_output(model, &output);
    check_dynamic_output(plotter, model, &output);
}

// Checks the properties of the common output, i.e. the outputs that are independent of the simulation mode
fn check_common_output(model: &BowModel, output: &BowResult) {
    let Common { limb, layers, string_length, string_stiffness, string_mass, limb_mass } = &output.common;
    let LimbInfo { length, position_eval, position_control: _, width, height, bounds, ratio: _, heights: _ } = &limb;    // TODO: Come back to this once the contents of this struct are finalized

    // There must be as many lengths as there are limb evaluation points defined in the model
    // Lengths must be sorted in strictly ascending order and start at zero
    assert!(length.len() == model.settings.num_limb_eval_points);
    assert!(length.iter().tuple_windows().all(|(a, b)| a < b));
    assert!(length[0] == 0.0);

    // Number of positions, widths, heights and bounds must match number of evaluation points
    assert!(position_eval.len() == model.settings.num_limb_eval_points);
    assert!(width.len() == model.settings.num_limb_eval_points);
    assert!(height.len() == model.settings.num_limb_eval_points);
    assert!(bounds.len() == model.settings.num_limb_eval_points);

    // Width and height must be strictly positive
    assert!(width.iter().all(|&w| w > 0.0));
    assert!(height.iter().all(|&h| h > 0.0));

    // Number of layer bounds must be consistent with the number of layers defined in the model
    assert!(bounds.iter().all(|b| b.len() == model.section.layers.len() + 1));

    // Number of layers must match the number of layers defined in the model
    // Layer info doesn't contain much currently, but the layer names must not be empty
    assert!(layers.len() == model.section.layers.len());
    for layer in layers {
        let LayerInfo { name, color, maximum_stresses, allowed_stresses, maximum_strains, allowed_strains } = layer;
        assert!(!name.is_empty());
        assert!(!color.is_empty());
        assert!(maximum_stresses.0 >= 0.0);
        assert!(maximum_stresses.1 >= 0.0);
        assert!(allowed_stresses.0 >= 0.0);
        assert!(allowed_stresses.1 >= 0.0);
        assert!(maximum_strains.0 >= 0.0);
        assert!(maximum_strains.1 >= 0.0);
        assert!(allowed_strains.0 >= 0.0);
        assert!(allowed_strains.1 >= 0.0);
    }

    // String length, stiffness, mass and limb mass must be positive
    assert!(*string_length > 0.0);
    assert!(*string_stiffness > 0.0);
    assert!(*string_mass > 0.0);
    assert!(*limb_mass > 0.0);
}

fn check_static_output(model: &BowModel, output: &BowResult) {
    check_general_state_properties(model, &output.statics.as_ref().unwrap().states);
    check_static_state_properties(model, output);
    check_static_state_physics(model, output);
    check_static_derivatives(model, output);
    check_static_scalar_results(model, output);
}

fn check_dynamic_output(plotter: &mut Plotter, model: &BowModel, output: &BowResult) {
    check_general_state_properties(model, &output.dynamics.as_ref().unwrap().states);
    check_dynamic_state_properties(plotter, model, output);
    check_dynamic_derivatives(model, output);
    check_dynamic_scalar_results(model, output);
}

// Check some basic properties (domain, dimensions) for the scalar static outputs
fn check_static_scalar_results(model: &BowModel, output: &BowResult) {
    let Statics {
        states,
        final_draw_force,
        final_drawing_work,
        storage_factor,
        max_forces,
        max_stresses
    } = output.statics.as_ref().unwrap();

    // Draw force, drawing work and storage factor must be positive
    assert!(*final_draw_force > 0.0);
    assert!(*final_drawing_work > 0.0);
    assert!(*storage_factor > 0.0);

       // Check basic properties of maximum forces
    check_max_forces(max_forces, states);

    // Check basic properties of maximum stresses/strains
    check_max_stresses(max_stresses, states, model);
}

// Check some basic properties (domain, dimensions) for the scalar dynamic outputs
fn check_dynamic_scalar_results(model: &BowModel, output: &BowResult) {
    let Dynamics {
        states,
        arrow_departure,
        max_forces,
        max_stresses
    } = output.dynamics.as_ref().unwrap();

    if let Some(arrow_departure) = arrow_departure {
        let ArrowDeparture { state_idx, arrow_pos, arrow_vel, kinetic_energy_arrow, elastic_energy_limbs, kinetic_energy_limbs, damping_energy_limbs, elastic_energy_string, kinetic_energy_string, damping_energy_string, energy_efficiency } = arrow_departure;

        // Check if the quantities at separation are consistent with the states and the index
        assert!(*arrow_pos == states.arrow_pos[*state_idx]);
        assert!(*arrow_vel == states.arrow_vel[*state_idx]);
        assert!(*kinetic_energy_arrow == states.kinetic_energy_arrow[*state_idx]);
        assert!(*elastic_energy_limbs == states.elastic_energy_limbs[*state_idx]);
        assert!(*kinetic_energy_limbs == states.kinetic_energy_limbs[*state_idx]);
        assert!(*damping_energy_limbs == states.damping_energy_limbs[*state_idx]);
        assert!(*elastic_energy_string == states.elastic_energy_string[*state_idx]);
        assert!(*kinetic_energy_string == states.kinetic_energy_string[*state_idx]);
        assert!(*damping_energy_string == states.damping_energy_string[*state_idx]);

        // Check range of the energy efficiency
        assert!(*energy_efficiency > 0.0);
        assert!(*energy_efficiency < 1.0);

        // Up to the departure of the arrow, its position and velocity are equal to that of the string
        for i in 0..=*state_idx {
            assert!(states.arrow_pos[i] == states.string_pos[i][0][1]);
            assert!(states.arrow_vel[i] == states.string_vel[i][0][1]);
        }

        // After the departure, position and velocity are no longer equal
        // (The probability that one of those is equal by chance is very low)
        for i in state_idx+1..states.len() {
            assert!(states.arrow_pos[i] != states.string_pos[i][0][1]);
            assert!(states.arrow_vel[i] != states.string_vel[i][0][1]);
        }
    }
    else {
        // If no arrow departure occurred, the arrow position and velocity must be equal to the string throughout
        for i in 0..=states.len() {
            assert!(states.arrow_pos[i] == states.string_pos[i][0][1]);
            assert!(states.arrow_vel[i] == states.string_vel[i][0][1]);
        }
    }

    // Check basic properties of maximum forces
    check_max_forces(max_forces, states);

    // Check basic properties of maximum stresses/strains
    check_max_stresses(max_stresses, states, model);
}

fn check_max_forces(max_forces: &MaxForces, states: &StateVec) {
    let MaxForces { max_string_force, max_strand_force, max_draw_force, min_grip_force, max_grip_force } = max_forces;

    // The maximum string force must be positive and occur within the total number of states
    assert!(max_string_force.0 > 0.0);
    assert!(max_string_force.1 < states.len());

    // The maximum strand force must be smaller or equal to the maximum string force, because there are 1 or more strands.
    // It must occur within the total number of states
    assert!(max_strand_force.0 <= max_string_force.0);
    assert!(max_string_force.1 < states.len());

    // The maximum string force must be positive or zero (dynamics) and occur within the total number of states
    assert!(max_draw_force.0 >= 0.0);
    assert!(max_draw_force.1 < states.len());

    // The minimum and maximum grip force must occur within the total number of states
    // They may be positive or negative (since the duration of the simulation might not include any sign changes)
    assert!(min_grip_force.1 < states.len());
    assert!(max_grip_force.1 < states.len());
}

fn check_max_stresses(max_stresses: &MaxStresses, states: &StateVec, model: &BowModel) {
    let MaxStresses { max_layer_stress_tension, max_layer_stress_compression, max_layer_strain_tension, max_layer_strain_compression } = max_stresses;

    // There must be as many min layer stress entries as there are layers in the model
    // The indices must be in the correct range (state, length, belly/back)
    assert!(max_layer_stress_tension.len() == model.section.layers.len());
    for layer_stress in max_layer_stress_tension {
        assert!(layer_stress.1[0] < states.len());
        assert!(layer_stress.1[1] < model.settings.num_limb_eval_points);
        assert!(layer_stress.1[2] < 2);
    }

    // There must be as many max layer stress entries as there are layers in the model
    // The indices must be in the correct range (state, length, belly/back)
    assert!(max_layer_stress_compression.len() == model.section.layers.len());
    for layer_stress in max_layer_stress_compression {
        assert!(layer_stress.1[0] < states.len());
        assert!(layer_stress.1[1] < model.settings.num_limb_eval_points);
        assert!(layer_stress.1[2] < 2);
    }

    // Same for min strains
    assert!(max_layer_strain_tension.len() == model.section.layers.len());
    for layer_strain in max_layer_strain_tension {
        assert!(layer_strain.1[0] < states.len());
        assert!(layer_strain.1[1] < model.settings.num_limb_eval_points);
        assert!(layer_strain.1[2] < 2);
    }

    // Same for max strains
    assert!(max_layer_strain_compression.len() == model.section.layers.len());
    for layer_strain in max_layer_strain_compression {
        assert!(layer_strain.1[0] < states.len());
        assert!(layer_strain.1[1] < model.settings.num_limb_eval_points);
        assert!(layer_strain.1[2] < 2);
    }
}

// Check basic properties for a series of static or dynamic bow states
fn check_general_state_properties(model: &BowModel, states: &StateVec) {
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
            elastic_energy_limbs,
            elastic_energy_string,
            kinetic_energy_limbs,
            kinetic_energy_string,
            kinetic_energy_arrow,
            damping_energy_limbs,
            damping_energy_string,
            damping_power_limbs,
            damping_power_string,
            draw_force: _,
            draw_stiffness: _,
            grip_force: _,
            string_length: _,
            string_tip_angle: _,
            string_center_angle: _,
            string_force: _,
            strand_force: _
        } = state.to_owned();

        // Time must be non-negative
        assert!(time >= 0.0);

        // Draw length must not exceed dimension setting
        assert!(draw_length <= model.dimensions.draw_length);

        // Limb position and velocity must have as many entries as there are eval points
        assert!(limb_pos.len() == model.settings.num_limb_eval_points);
        assert!(limb_vel.len() == model.settings.num_limb_eval_points);

        // Limb starting point (positions and angle) must match the dimension settings
        assert_abs_diff_eq!(limb_pos[0][0], 0.5*model.dimensions.handle_length, epsilon=1e-12);
        assert_abs_diff_eq!(limb_pos[0][1], model.dimensions.handle_offset, epsilon=1e-12);
        assert_abs_diff_eq!(limb_pos[0][2], model.dimensions.handle_angle, epsilon=1e-12);

        // String position and velocity must have at least 2 entries and up to the maximum number of contact points defined by the number of limb elements
        // The position of the first string point must be consistent with the draw length
        assert!(string_pos.len() >= 2 && string_pos.len() <= model.settings.num_limb_elements + 2);
        assert!(string_vel.len() >= 2 && string_vel.len() <= model.settings.num_limb_elements + 2);
        assert!(string_pos[0][1] == -draw_length);

        // Limb strains and forces must have as many entries as there are eval points
        assert!(limb_strain.len() == model.settings.num_limb_eval_points);
        assert!(limb_force.len() == model.settings.num_limb_eval_points);

        // Layer stresses and strains must contain one vector for each layer.
        // Each of those vectors must contain as many entries as there are eval points.
        assert!(layer_strain.len() == model.section.layers.len());
        assert!(layer_stress.len() == model.section.layers.len());
        assert!(layer_strain.iter().all(|x| x.len() == model.settings.num_limb_eval_points));
        assert!(layer_stress.iter().all(|x| x.len() == model.settings.num_limb_eval_points));

        // All energies and powers must be non-negative (with a small tolerance if needed)
        assert!(elastic_energy_limbs >= 0.0);
        assert!(elastic_energy_string >= 0.0);
        assert!(kinetic_energy_limbs >= 0.0);
        assert!(kinetic_energy_string >= 0.0);
        assert!(kinetic_energy_arrow >= 0.0);
        assert!(damping_energy_limbs >= -1e-12);
        assert!(damping_energy_string >= -1e-12);
        assert!(damping_power_limbs >= 0.0);
        assert!(damping_power_string >= 0.0);
    }
}

// Check basic properties that are specific to a series of static bow states
// TODO: Combine with check for static equilibrium?
fn check_static_state_properties(model: &BowModel, output: &BowResult) {
    let statics = output.statics.as_ref().unwrap();
    let states = &statics.states;

    let ABS_TOL_FORCE = 1e-3*statics.final_draw_force;

    // First draw length must be equal to specified brace height, last draw length to specified draw length
    // The number of states must currently equal the minimum draw resolution (+1 because steps vs. points) since step size control isn't implemented yet
    // The states must be ordered by strictly increasing draw length
    assert_abs_diff_eq!(*states.draw_length.first().unwrap(), model.dimensions.brace_height, epsilon=1e-15);
    assert_abs_diff_eq!(*states.draw_length.last().unwrap(), model.dimensions.draw_length, epsilon=1e-15);
    assert!(states.draw_length.len() == model.settings.min_draw_resolution + 1);
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
            elastic_energy_limbs: _,
            elastic_energy_string: _,
            kinetic_energy_limbs,
            kinetic_energy_string,
            kinetic_energy_arrow,
            damping_energy_limbs,
            damping_energy_string,
            damping_power_limbs,
            damping_power_string,
            draw_force,
            draw_stiffness: _,
            string_length: _,
            string_tip_angle: _,
            string_center_angle: _,
            grip_force,
            string_force,
            strand_force
        } = state.to_owned();

        // Time must be zero
        assert!(time == 0.0);

        // Draw length must range from brace height to full draw
        assert!(draw_length >= model.dimensions.brace_height);
        assert!(draw_length <= model.dimensions.draw_length);

        // Limb and string velocities must be zero
        assert!(limb_vel.iter().all(SVector::is_zero));
        assert!(string_vel.iter().all(SVector::is_zero));

        // Arrow position must be identical to negative draw length, velocity and acceleration must be zero
        assert!(arrow_pos == -draw_length);
        assert!(arrow_vel == 0.0);
        assert!(arrow_acc == 0.0);

        // Kinetic energies, damping powers and damping energies must be zero
        assert!(kinetic_energy_limbs == 0.0);
        assert!(kinetic_energy_string == 0.0);
        assert!(kinetic_energy_arrow == 0.0);
        assert!(damping_energy_limbs == 0.0);
        assert!(damping_energy_string == 0.0);
        assert!(damping_power_limbs == 0.0);
        assert!(damping_power_string == 0.0);

        // Draw force, string force and strand force must be positive
        assert!(draw_force >= -ABS_TOL_FORCE);  // Allow slightly negative draw force because static equilibrium at the braced state is not 100% accurate
        assert!(string_force >= 0.0);
        assert!(strand_force >= 0.0);

        // Grip force must be identical to negative draw force
        assert_abs_diff_eq!(grip_force, draw_force, epsilon=ABS_TOL_FORCE);
    }
}

// Check basic properties that are specific to a series of static bow states
fn check_dynamic_state_properties(plotter: &mut Plotter, model: &BowModel, output: &BowResult) {
    let dynamics = output.dynamics.as_ref().unwrap();
    let states = &dynamics.states;

    // Total energy in the system at t = 0, to be used for comparisons later
    let TOTAL_ENERGY_REF = states.elastic_energy_limbs[0] + states.elastic_energy_string[0] + states.kinetic_energy_limbs[0] + states.kinetic_energy_string[0] + states.kinetic_energy_arrow[0] + states.damping_energy_limbs[0] + states.damping_energy_string[0];
    let TOTAL_ENERGY_ABS_TOL = 1e-2*TOTAL_ENERGY_REF;

    let ABS_TOL_TIMESTEP = 1e-12;
    let ABS_TOL_ARROW_ACC = 1e-9*states.arrow_acc[0];

    // Time of the first state must be zero.
    // Timesteps between states must lie between the minimum and maximum defined in the settings,
    // except for the last step before arrow separation or the end of the simulation, which are allowed to be shorter.
    assert!(states.time[0] == 0.0);
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
    assert_abs_diff_eq!(states.arrow_pos[0], -model.dimensions.draw_length, epsilon=1e-15);
    assert!(states.arrow_vel[0] == 0.0);

    for state in states.iter() {
        let State {
            time,
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
            arrow_vel,
            arrow_acc,
            elastic_energy_limbs,
            elastic_energy_string,
            kinetic_energy_limbs,
            kinetic_energy_string,
            kinetic_energy_arrow,
            damping_energy_limbs,
            damping_energy_string,
            damping_power_limbs,
            damping_power_string,
            draw_force,
            draw_stiffness,
            grip_force: _,
            string_length: _,
            string_tip_angle: _,
            string_center_angle: _,
            string_force: _,
            strand_force: _
        } = state.to_owned();

        // Arrow position must not exceed draw length
        assert!(arrow_pos >= -model.dimensions.draw_length);

        // Arrow acceleration must be larger than critical acceleration due to clamp force
        assert!(arrow_acc >= -model.settings.arrow_clamp_force/model.masses.arrow - ABS_TOL_ARROW_ACC);

        // Draw force and draw stiffness must be zero
        assert!(draw_force == 0.0);
        assert!(draw_stiffness == 0.0);

        // If the string has a non-zero damping ratio and the system is in motion, the damping power and energy must be positive (and otherwise zero).
        if arrow_vel > 0.0 && model.damping.damping_ratio_string != 0.0 {
            assert!(damping_power_string > 0.0);
            assert!(damping_energy_string > 0.0);
        } else {
            assert!(damping_power_string == 0.0);
            assert!(damping_energy_limbs == 0.0);
        }

        // If the limbs have a non-zero damping ratio and the system is in motion, the damping power and energy must be positive (and otherwise zero).
        if arrow_vel > 0.0 && model.damping.damping_ratio_limbs != 0.0 {
            assert!(damping_power_limbs > 0.0);
        } else {
            assert!(damping_power_limbs == 0.0);
        }

        // Check if the sum of all energies stays constant
        let total_energy = elastic_energy_limbs + elastic_energy_string + kinetic_energy_limbs + kinetic_energy_string + kinetic_energy_arrow + damping_energy_limbs + damping_energy_string;
        assert_abs_diff_eq!(total_energy, TOTAL_ENERGY_REF, epsilon=TOTAL_ENERGY_ABS_TOL);

        // Todo: Find a way to check dynamics, i.e. Force = Mass x Acceleration

        plotter.add_point((time, damping_energy_limbs), (time, 0.0), "Damping energy limbs", "Time [s]", "Energy [J]");
        plotter.add_point((time, damping_energy_string), (time, 0.0), "Damping energy string", "Time [s]", "Energy [J]");

        plotter.add_point((time, damping_power_limbs), (time, 0.0), "Damping power limbs", "Time [s]", "Energy [J]");
        plotter.add_point((time, damping_power_string), (time, 0.0), "Damping power string", "Time [s]", "Energy [J]");

        let total_energy = elastic_energy_limbs + elastic_energy_string + kinetic_energy_limbs + kinetic_energy_string + kinetic_energy_arrow + damping_energy_limbs + damping_energy_string;
        plotter.add_point((time, total_energy), (time, 0.0), "Total energy", "Time [s]", "Energy [J]");
    }
}

// Checks if the section forces and stresses are in equilibrium with the external forces of the bow
// TODO: Document/update this test in the theory manual, especially the definition of the string angle
fn check_static_state_physics(model: &BowModel, output: &BowResult) {
    let statics = output.statics.as_ref().unwrap();
    let states = &statics.states;

    let ABS_TOL_ALPHA = 1e-6;                                                           // Tolerance for the string angle in braced state
    let ABS_TOL_FORCE = 1e-3*statics.final_draw_force;                                  // Tolerance for force comparisons
    let ABS_TOL_MOMENT = 1e-3*statics.final_draw_force*model.dimensions.draw_length;    // Tolerance for moment comparisons
    let ABS_TOL_ENERGY = 0.5e-2*states.elastic_energy_limbs[0];                         // Tolerance for energy comparisons

    // Perform checks on each static state
    // i: State
    // j: Length along limb
    // k: Cross section layer
    for (i, state) in states.iter().enumerate() {
        // Analytical values for the draw force and grip force according to the string force, string angle and static considerations
        let alpha = 0.5*state.string_center_angle;  // f64::atan((string_pos_b[1] - string_pos_a[1])/(string_pos_b[0] - string_pos_a[0]));
        let draw_force_ref = 2.0*state.string_force*f64::cos(alpha);   // Reference draw force according to the string force
        let grip_force_ref = draw_force_ref;                            // Grip force is equal to string force in the static case

        // Compare analytical string and grip forces to reference values
        assert_abs_diff_eq!(*state.draw_force, draw_force_ref, epsilon=ABS_TOL_FORCE);
        assert_abs_diff_eq!(*state.grip_force, grip_force_ref, epsilon=ABS_TOL_FORCE);

        // For the first state, the string angle alpha must be 90°
        if i == 0 {
            assert_abs_diff_eq!(alpha, FRAC_PI_2, epsilon=ABS_TOL_ALPHA);
        }

        // Actual drawing work as elastic energy of limb and string compared to the initial (braced) state
        let drawing_work = *state.elastic_energy_limbs + *state.elastic_energy_string - (states.elastic_energy_limbs[0] + states.elastic_energy_string[0]);

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
                let Q_out = state.limb_force[j][1];
                let M_out = state.limb_force[j][2];

                // Calculate the cross section's normal force and bending moment by integrating the normal stresses over the cross section.
                // Compare the integrated forces to those from the simulation output to check if the stresses are consistent with the section forces.
                // (Shear force is not calculated since the shear stress is not yet part of the output results)

                let bounds = &output.common.limb.bounds[j];
                let width = output.common.limb.width[j];
                let mut forces = SVector::zeros();

                for (k, (&y_back, &y_belly)) in bounds.iter().tuple_windows().enumerate() {
                    // Function of normal stress sigma and its moment sigma*y over the layer's height coordinate y
                    let stresses = |y: f64| {
                        let sigma_back = state.layer_stress[k][j][0];
                        let sigma_belly = state.layer_stress[k][j][1];
                        let sigma_y = sigma_back + (y - y_back)/(y_belly - y_back)*(sigma_belly - sigma_back);
                        width*vector![sigma_y, -y*sigma_y]
                    };

                    if y_back != y_belly {
                        forces += fixed_simpson(stresses, y_belly, y_back, 100);
                    }
                }

                // Cross-section forces according to integration of the stresses
                let N_int = forces[0];
                let M_int = forces[1];

                assert_relative_eq!(N_int, N_out, max_relative=ABS_TOL_FORCE);
                assert_relative_eq!(M_int, M_out, max_relative=ABS_TOL_MOMENT);

                // The next checks verify that the cross-section forces are in balance with the external force
                // that the string exerts on the bow limb.

                // Limb endpoint
                let x_contact = state.string_pos[1][0];
                let y_contact = state.string_pos[1][1];

                // Cartesian components of the string force
                let Fx = -state.string_force*f64::sin(alpha);
                let Fy = -state.string_force*f64::cos(alpha);

                // Current position on the profile curve
                let x = state.limb_pos[j][0];
                let y = state.limb_pos[j][1];
                let φ = state.limb_pos[j][2];

                // Cross-section according to static equilibrium with the string force
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
fn check_dynamic_derivatives(model: &BowModel, output: &BowResult) {
    let dynamics = output.dynamics.as_ref().unwrap();
    let states = &dynamics.states;

    let V_MAX_ARROW = discrete_maximum_1d(&states.arrow_vel).0;              // Maximum arrow velocity as reference for comparison
    let A_MAX_ARROW = discrete_maximum_1d(&states.arrow_acc).0;              // Maximum arrow acceleration as reference for comparison
    let W_MAX_STRING = V_MAX_ARROW/(output.common.string_length/2.0);        // Estimated maximum angular velocity of the string

    let P_MAX_ARROW = model.masses.arrow*A_MAX_ARROW*V_MAX_ARROW;            // Maximum power of arrow acceleration
    let P_MAX_LIMBS = discrete_maximum_1d(&states.damping_power_limbs).0;    // Maximum damping power of the limbs as reference for comparison
    let P_MAX_STRING = discrete_maximum_1d(&states.damping_power_limbs).0;    // Maximum damping power of the string as reference for comparison


    let ABS_TOL_VEL_LINEAR = 1e-3*V_MAX_ARROW;
    let ABS_TOL_ACC_LINEAR = 1e-3*A_MAX_ARROW;
    let ABS_TOL_VEL_ANGULAR = 1e-3*W_MAX_STRING;

    let ABS_TOL_POWER_ARROW = 1e-3*P_MAX_ARROW;
    let ABS_TOL_POWER_LIMBS = 1e-3*P_MAX_LIMBS;
    let ABS_TOL_POWER_STRING = 1e-3*P_MAX_STRING;

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
            elastic_energy_limbs: _,
            elastic_energy_string: _,
            kinetic_energy_limbs: _,
            kinetic_energy_string: _,
            kinetic_energy_arrow: kinetic_energy_arrow0,
            damping_energy_limbs: damping_energy_limbs0,
            damping_energy_string: damping_energy_string0,
            damping_power_limbs: damping_power_limbs0,
            damping_power_string: damping_power_string0,
            draw_force: _,
            draw_stiffness: _,
            grip_force: _,
            string_length: _,
            string_tip_angle: _,
            string_center_angle: _,
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
            elastic_energy_limbs: _,
            elastic_energy_string: _,
            kinetic_energy_limbs: _,
            kinetic_energy_string: _,
            kinetic_energy_arrow: kinetic_energy_arrow1,
            damping_energy_limbs: damping_energy_limbs1,
            damping_energy_string: damping_energy_string1,
            damping_power_limbs: damping_power_limbs1,
            damping_power_string: damping_power_string1,
            draw_force: _,
            draw_stiffness: _,
            grip_force: _,
            string_length: _,
            string_tip_angle: _,
            string_center_angle: _,
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

        // Check if the numerical derivative of the arrow's kinetic energy (=power) equals the acceleration force times velocity
        // Skip this check if the separation of the arrow from the string occured between the states
        if dynamics.arrow_departure.as_ref().map(|x| x.state_idx) != Some(index) {
            let kinetic_power_arrow_num = (kinetic_energy_arrow1 - kinetic_energy_arrow0)/(time1 - time0);
            let kinetic_power_arrow_avg = 0.5*model.masses.arrow*(arrow_acc0*arrow_vel0 + arrow_acc1*arrow_vel1);
            assert_abs_diff_eq!(kinetic_power_arrow_avg, kinetic_power_arrow_num, epsilon=ABS_TOL_POWER_ARROW);
        }

        // Check if the numerical derivatives of the limb's damping energy matches the corresponding damping power
        let damping_power_limbs_num = (damping_energy_limbs1 - damping_energy_limbs0)/(time1 - time0);
        let damping_power_limbs_avg = 0.5*(damping_power_limbs0 + damping_power_limbs1);
        assert_abs_diff_eq!(damping_power_limbs_avg, damping_power_limbs_num, epsilon=ABS_TOL_POWER_LIMBS);

        // Check if the numerical derivatives of the string's damping energy matches the corresponding damping power
        let damping_power_string_num = (damping_energy_string1 - damping_energy_string0)/(time1 - time0);
        let damping_power_string_avg = 0.5*(damping_power_string0 + damping_power_string1);
        assert_abs_diff_eq!(damping_power_string_avg, damping_power_string_num, epsilon=ABS_TOL_POWER_STRING);
    }
}

// Checks the derivatives wrt. draw length in a series of static bow states, i.e. energy, force and stiffness,
// by comparing them to finite difference approximations from the original data.
fn check_static_derivatives(_model: &BowModel, output: &BowResult) {
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
            elastic_energy_limbs: elastic_energy_limbs0,
            elastic_energy_string: elastic_energy_string0,
            kinetic_energy_limbs: _,
            kinetic_energy_string: _,
            kinetic_energy_arrow: _,
            damping_energy_limbs: _,
            damping_energy_string: _,
            damping_power_limbs: _,
            damping_power_string: _,
            draw_force: draw_force0,
            draw_stiffness: draw_stiffness0,
            grip_force: _,
            string_length: _,
            string_tip_angle: _,
            string_center_angle: _,
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
            elastic_energy_limbs: elastic_energy_limbs1,
            elastic_energy_string: elastic_energy_string1,
            kinetic_energy_limbs: _,
            kinetic_energy_string: _,
            kinetic_energy_arrow: _,
            damping_energy_limbs: _,
            damping_energy_string: _,
            damping_power_limbs: _,
            damping_power_string: _,
            draw_force: draw_force1,
            draw_stiffness: draw_stiffness1,
            grip_force: _,
            string_length: _,
            string_tip_angle: _,
            string_center_angle: _,
            string_force: _,
            strand_force: _
        } = state1.to_owned();

        // Only consider states where the number of string contact points has not changed from one state to the next, otherwise we can't differentiate.
        if string_pos0.len() == string_pos1.len() {
            // Compare numerical draw force by differentiation of the total energy to the average draw force in the interval
            let draw_force_num = ((elastic_energy_limbs1 + elastic_energy_string1) - (elastic_energy_limbs0 + elastic_energy_string0))/(draw_length1 - draw_length0);
            let draw_force_avg = 0.5*(draw_force0 + draw_force1);
            assert_abs_diff_eq!(draw_force_avg, draw_force_num, epsilon=ABS_TOL_DRAW_FORCE);

            // Compare numerical draw stiffness (forward difference) to average stiffness in the interval
            let draw_stiffness_num = (draw_force1 - draw_force0)/(draw_length1 - draw_length0);
            let draw_stiffness_avg = 0.5*(draw_stiffness0 + draw_stiffness1);
            assert_abs_diff_eq!(draw_stiffness_avg, draw_stiffness_num, epsilon=ABS_TOL_DRAW_STIFFNESS);
        }
    }
}
