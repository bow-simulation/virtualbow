use std::f64::consts::{PI, FRAC_PI_2};
use clap::ValueEnum;
use itertools::Itertools;
use nalgebra::{SVector, vector};
use virtualbow_num::fem::solvers::eigen::{Mode, natural_frequencies};
use virtualbow_num::fem::solvers::statics::StaticSolver;
use virtualbow_num::fem::system::element::Element;
use virtualbow_num::fem::system::node::Node;
use virtualbow_num::fem::system::system::{System, SystemEval};
use crate::errors::ModelError;
use crate::geometry::{DiscreteLimbGeometry, LimbGeometry};
use crate::input::BowModel;
use crate::output::{Dynamics, LayerInfo, LimbInfo, BowResult, Common, State, StateVec, Statics, ArrowDeparture};
use virtualbow_num::fem::elements::beam::beam::BeamElement;
use virtualbow_num::fem::elements::mass::MassElement;
use virtualbow_num::fem::elements::string::StringElement;
use virtualbow_num::fem::solvers::dynamics::{DynamicSolver, DynamicSolverSettings, StopCondition, TimeStepping};
use virtualbow_num::utils::integration::cumulative_simpson;
use virtualbow_num::utils::newton;
use virtualbow_num::utils::roots::find_root_falsi;
use virtualbow_num::utils::minmax::{discrete_maximum_1d, discrete_maximum_nd, discrete_minimum_1d, discrete_minimum_nd};

#[derive(ValueEnum, PartialEq, Debug, Copy, Clone)]
pub enum SimulationMode {
    Static,
    Dynamic
}

pub struct Simulation<'a> {
    input: &'a BowModel,
    geometry: DiscreteLimbGeometry,

    limb_nodes: Vec<Node>,
    limb_elements: Vec<usize>,

    string_nodes: Vec<Node>,
    string_element: usize,

    mass_element_arrow: usize,            // Arrow mass element positioned at the string center
    mass_element_limb_tip: usize,         // Mass elememt at the end of the limb
    mass_element_string_center: usize,    // Mass element at the center of the string
    mass_element_string_tip: usize,       // Mass element at the end of the string

    // None if the arrow is still attached to the string
    // Otherwise the state index, time, position and velocity at separation from the string
    arrow_departure: Option<(usize, f64, f64, f64)>,
}

impl<'a> Simulation<'a> {
    // Numerical constants for the bracing simulation
    // TODO: Maybe put all those numerical parameters into a separate struct
    const BRACING_DELTA_START: f64 = 1e-3;       // Initial decrement in string length
    const BRACING_DELTA_MIN: f64 = 1e-5;         // Minimum step length, abort if smaller
    const BRACING_SLOPE_TOL: f64 = 1e-6;         // Tolerance for the string's slope error
    const BRACING_MAX_ROOT_ITER: usize = 20;     // Maximum number of iterations for the terminal root finding algorithm
    const BRACING_TARGET_ITER: usize = 5;        // Desired number of iterations for the static solver

    // Set up the simulation either with or without string and with or without damping, depending on simulation mode.
    fn initialize(input: &'a BowModel, string: bool, damping: bool) -> Result<(System, Simulation<'a>, Common), ModelError> {
        // Check basic validity of the model data and propagate any errors
        input.validate()?;

        // Create bow geometry from input,
        let geometry = LimbGeometry::new(input)?;

        // Layer setup data
        let layers = input.section.layers.iter().map(|layer| {
            LayerInfo {
                name: layer.name.clone()
            }
        }).collect_vec();

        // Discretize geometry into evaluation points and elements
        let geometry = geometry.discretize(input.settings.num_limb_eval_points, input.settings.num_limb_elements);

        let elements = geometry.segments.iter().map(BeamElement::new);

        let mut system = System::new();

        let limb_nodes: Vec<Node> = geometry.p_nodes.iter().enumerate().map(|(i, u)| {
            system.create_node(u, &[i != 0; 3])    // First node is fixed, all others
        }).collect();

        let limb_elements: Vec<usize> = elements.into_iter().enumerate().map(|(i, element)| {
            system.add_element(&[limb_nodes[i], limb_nodes[i+1]], element)
        }).collect();

        // Limb tip mass, required for limb damping calculation
        let mass_element_limb_tip = system.add_element(&[*limb_nodes.last().unwrap()], MassElement::new(input.masses.limb_tip));

        // If damping properties are to be initialized and the specified damping ratio for the limb is not zero,
        // perform a modal analysis of the limb without string and set the damping parameter of the beam elements according to the desired damping ratio.
        if damping && input.damping.damping_ratio_limbs != 0.0 {
            let modes = natural_frequencies(&mut system).map_err(ModelError::SimulationEigenSolutionFailed)?;
            let alpha = 2.0*input.damping.damping_ratio_limbs/modes[0].omega;
            for &e in &limb_elements {
                system.element_mut::<BeamElement>(e).set_damping(alpha);
            }
        }

        // Additional setup data

        // String center node that is fixed in the case of no string.
        // The rest of the string nodes come from the limb.
        let string_center = system.create_node(&vector![0.0, -input.dimensions.brace_height, 0.0], &[false, string, false]);
        let mut string_nodes = vec![string_center];  // TODO: Preallocate
        string_nodes.extend_from_slice(&limb_nodes);

        // Arrow and string mass elements, placed at the string center and endpoint
        // The arrow mass is halfed because of the symmetrical bow model.
        // The value of the string masses can only be set after the length of the string has been determined (only if the string is to be initialized at all).
        let mass_element_arrow = system.add_element(&[string_nodes[0]], MassElement::new(0.5*input.masses.arrow));
        let mass_element_string_center = system.add_element(&[string_nodes[0]], MassElement::new(0.0));
        let mass_element_string_tip = system.add_element(&[*limb_nodes.last().unwrap()], MassElement::new(0.0));

        // The string element only gets non-zero parameters if the string option is true
        let EA = if string { (input.string.n_strands as f64)*input.string.strand_stiffness } else { 0.0 };
        let ρA = if string { (input.string.n_strands as f64)*input.string.strand_density } else { 0.0 };

        let mut offsets = vec![0.0];                              // Offset at the string node is zero TODO: Preallocate
        offsets.extend(geometry.y_nodes.iter().map(|y| y[0]));    // Offsets between the limb nodes and the belly surface of the limb
        let string_element = StringElement::new(EA, 0.0, 1.0, 1.0, offsets);    // Damping is determined later when the length of the string is known, compression factor is set in dynamic analysis
        let string_element = system.add_element(&string_nodes, string_element);

        // Evaluate the string element so that the actual string length is computed
        // Then set the initial length to the current length so that the string is tension-free.
        system.update_element(string_element);
        let element = system.element_mut::<StringElement>(string_element);
        let l0 = element.get_current_length();
        element.set_initial_length(l0);

        // If string is to be initialized, perform bracing simulation
        if string {
            // Direction of the applied force for displacement control
            system.add_force(string_nodes[0].y(), move |_t| { -1.0 });

            // Returns the slope of the string at the centerpoint against the x direction
            let get_string_slope = |system: &System| -> f64 {
                let mut string_pos = system.element_ref::<StringElement>(string_element).contact_positions();
                let pos0 = string_pos.next().unwrap();    // String must always have at least two contact nodes
                let pos1 = string_pos.next().unwrap();    // String must always have at least two contact nodes

                (pos1[1] - pos0[1])/(pos1[0] - pos0[0])
            };

            // Initial values for the string factor, the slope and the step size for iterating on the string factor
            let mut factor1 = 1.0;
            let mut slope1 = get_string_slope(&system);
            let mut delta = Self::BRACING_DELTA_START;

            // Abort if the initial slope is negative, which means that the supplied brace height is too
            if slope1 < 0.0 {
                // TODO: Determine the minimum required brace height and put it into the error message
                return Err(ModelError::SimulationBraceHeightTooLow(input.dimensions.brace_height));
            }

            // Function that applies the given string length to the bow, solves for static equilibrium with the string pinned at brace height.
            // Returns the slope of the string as well as the return state of the static solver.
            // The root of this function is the string length that braces the bow with the desired brace height.
            let mut try_string_length = |factor: f64| {
                system.element_mut::<StringElement>(string_element).set_initial_length(factor*l0);

                let mut solver = StaticSolver::new(&mut system, newton::NewtonSettings::default());    // TODO: Don't construct new solver in each iteration
                let result = solver.equilibrium_displacement_controlled(string_nodes[0].y(), -input.dimensions.brace_height);
                let slope = get_string_slope(&system);

                (slope, result)
            };

            // Iterate on the string length factor in order to find the braced equilibrium state
            loop {
                // Try new factor reduced by step size
                let factor2 = factor1 - delta;
                let (slope2, result) = try_string_length(factor1 - delta);

                if let Ok(info) = result {
                    // Static iteration success
                    if slope2 <= 0.0 {
                        // Sign change of the slope: Almost done, do the rest by root finding
                        let try_string_length = |factor| { try_string_length(factor).0 };  // TODO: Error handling, do something with the solver information
                        find_root_falsi(try_string_length, factor1, factor2, slope1, slope2, 0.0, Self::BRACING_SLOPE_TOL, Self::BRACING_MAX_ROOT_ITER).ok_or(ModelError::SimulationBracingNoConvergence)?;
                        break;
                    }
                    else {
                        // Otherwise apply step and continue
                        factor1 = factor2;
                        slope1 = slope2;

                        // Adjust step size according to static solver performance
                        delta *= (Self::BRACING_TARGET_ITER as f64) / (info.iterations as f64);
                    }
                }
                else {
                    // Static iteration failure: Reduce step size by a generic factor
                    delta /= 2.0;
                }

                // Abort if the step size becomes too small
                if delta < Self::BRACING_DELTA_MIN {
                    return Err(ModelError::SimulationBracingNoSignChange)
                }
            }

            // After the string length is known, we can calculate the viscosity that is required
            // for achieving the prescribed string damping ratio
            let l0 = system.element_ref::<StringElement>(string_element).get_initial_length();
            let ηA = 4.0*l0/PI*f64::sqrt(ρA*EA)*input.damping.damping_ratio_string;
            system.element_mut::<StringElement>(string_element).set_linear_damping(ηA);

            // Base mass + additional masses of the string
            system.element_mut::<MassElement>(mass_element_string_center).set_mass(0.5*input.masses.string_center + 1.0/3.0*ρA*l0);
            system.element_mut::<MassElement>(mass_element_string_tip).set_mass(input.masses.string_tip + 2.0/3.0*ρA*l0);
        }

        // Compute additional common output results
        let string_length = 2.0*l0;                                                                                // Actual string length due to symmetry
        let string_stiffness = EA/string_length;                                                                   // Stiffness of the complete string from tip to tip
        let string_mass = 2.0*(ρA*l0 + input.masses.string_tip) + input.masses.string_center;                      // String mass including additional masses and symmetry
        let limb_mass = geometry.segments.iter().map(|segment| segment.m).sum::<f64>() + input.masses.limb_tip;    // Mass of a single limb, including additional masses

        // Simulation info object
        let simulation = Self {
            input,
            geometry,
            limb_nodes,
            limb_elements,
            string_nodes,
            string_element,
            mass_element_arrow,
            mass_element_string_center,
            mass_element_string_tip,
            mass_element_limb_tip,
            arrow_departure: None,
        };

        let common = Common {
            limb: LimbInfo {
                length: simulation.geometry.s_eval.clone(),
                position: simulation.geometry.p_eval.clone(),
                width: simulation.geometry.w_eval.clone(),
                height: simulation.geometry.h_eval.clone(),
                bounds: simulation.geometry.y_eval.iter().map(|y| y.data.clone().into()).collect(),  // TODO: Uglyyy
            },
            layers,
            string_length,
            string_stiffness,
            string_mass,
            limb_mass
        };

        Ok((system, simulation, common))
    }

    // Callback: (phase, progress) -> continue
    pub fn simulate<F>(model: &'a BowModel, mode: SimulationMode, mut callback: F) -> Result<BowResult, ModelError>
        where F: FnMut(SimulationMode, f64) -> bool
    {
        // Initialize simulation. String always, but damping only in dynamic mode (saves an einegvalue analysis).
        let (mut system, mut simulation, common) = Self::initialize(model, true, mode == SimulationMode::Dynamic)?;

        let statics = {
            // "Draw" the bow by solving for a static equilibrium path of the string node from brace height to full draw
            // and store each intermediate step in the static output.
            let mut states = StateVec::new();
            let mut solver = StaticSolver::new(&mut system, newton::NewtonSettings::default());

            solver.equilibrium_path_displacement_controlled(simulation.string_nodes[0].y(), -model.dimensions.draw_length, model.settings.min_draw_resolution, &mut |system, eval, stiffness| {
                let state = simulation.get_bow_state(system, eval, -2.0*stiffness);  // TODO: Why the sign flip of the stiffness?
                let progress = (state.draw_length - model.dimensions.brace_height)/(model.dimensions.draw_length - model.dimensions.brace_height);
                states.push(state);

                callback(SimulationMode::Static, 100.0*progress)
            }).map_err(ModelError::SimulationStaticSolutionFailed)?;

            // Compute additional static output values

            let draw_length_front = *states.draw_length.first().unwrap();
            let draw_length_back = *states.draw_length.last().unwrap();
            let draw_force_back = *states.draw_force.last().unwrap();
            let e_pot_front = states.elastic_energy_limbs.first().unwrap() + states.elastic_energy_string.first().unwrap();
            let e_pot_back = states.elastic_energy_limbs.last().unwrap() + states.elastic_energy_string.last().unwrap();

            let final_draw_force = draw_force_back;
            let final_drawing_work = e_pot_back - e_pot_front;
            let storage_factor = (e_pot_back - e_pot_front) / (0.5*(draw_length_back - draw_length_front)*draw_force_back);

            let max_string_force = discrete_maximum_1d(&states.string_force);
            let max_strand_force = (max_string_force.0/(model.string.n_strands as f64), max_string_force.1);
            let max_draw_force = discrete_maximum_1d(&states.draw_force);
            let min_grip_force = discrete_minimum_1d(&states.grip_force);
            let max_grip_force = discrete_maximum_1d(&states.grip_force);

            let min_layer_stresses = (0..model.section.layers.len()).map(|i_layer| find_min_layer_result(&states.layer_stress, i_layer)).collect();
            let max_layer_stresses = (0..model.section.layers.len()).map(|i_layer| find_max_layer_result(&states.layer_stress, i_layer)).collect();

            // Collect static outputs
            Statics {
                states,
                final_draw_force,
                final_drawing_work,
                storage_factor,
                max_string_force,
                max_strand_force,
                max_draw_force,
                min_grip_force,
                max_grip_force,
                min_layer_stresses,
                max_layer_stresses,
            }
        };

        // Perform dynamic simulation, if required
        let dynamics = {
            if mode == SimulationMode::Dynamic {
                // Estimate timeout after which to abort the simulation
                let k_bow = statics.final_draw_force/(model.dimensions.draw_length - model.dimensions.brace_height);
                let t_max = model.settings.timeout_factor*FRAC_PI_2*f64::sqrt(model.masses.arrow/k_bow);
                let step = TimeStepping::Adaptive{
                    min_timestep: model.settings.min_timestep,
                    max_timestep: model.settings.max_timestep,
                    steps_per_period: model.settings.steps_per_period
                };

                let settings = DynamicSolverSettings { time_stepping: step, max_time: t_max, ..Default::default() };
                let mut states = StateVec::new();

                // Modify the string's compression factor to make it a lot less stiff on compression
                system.element_mut::<StringElement>(simulation.string_element).set_compression_factor(model.settings.string_compression_factor);

                // Remove static draw force
                system.clear_forces();

                // Simulate the first part of the shot until either the arrow separates from the string
                // or the timeout is reached for some reason
                let stop_condition = StopCondition::Acceleration(simulation.string_nodes[0].y(), -model.settings.arrow_clamp_force/model.masses.arrow, -1);    // Condition for arrow separation

                let mut brace_crossing_time = f64::INFINITY;    // Time when the arrow crosses brace height, initially unknown
                let mut estimated = true;                       // Whether the time is estimated or already known
                let mut progress = 0.0;                         // Estimated simulation progress

                let mut solver = DynamicSolver::new(&mut system, settings);
                solver.solve(stop_condition, &mut |system, eval| {
                    // Evaluate current bow state
                    let state = simulation.get_bow_state(system, eval, 0.0);

                    // Only update the brace crossing time if it is estimated,
                    // no need to update once it is known
                    if estimated {
                        let ut = state.arrow_pos;                   // Current arrow travel
                        let u0 = -model.dimensions.draw_length;     // Arrow travel at full draw
                        let uT = -model.dimensions.brace_height;    // Arrow travel at brace height

                        if ut < uT {
                            // Arrow hasn't yet reached brace height: Update estimate for crossing time from current time and velocity
                            brace_crossing_time = FRAC_PI_2*state.time/f64::acos((ut - uT)/(u0 - uT));
                        }
                        else {
                            // Arrow has reached brace height: Set crossing time to current time and stop estimations
                            brace_crossing_time = state.time;
                            estimated = false;
                        }
                    }

                    // Estimate progress and use maximum to ensure that progress never decreases
                    progress = f64::max(progress, state.time/(model.settings.timespan_factor*brace_crossing_time));

                    // Add bow state to the results
                    states.push(state);

                    return callback(SimulationMode::Dynamic, 100.0*progress);
                }).map_err(ModelError::SimulationDynamicSolutionFailed)?;

                // Record arrow state at the time of separation from the string
                let state = states.iter().next_back().unwrap();
                simulation.arrow_departure = Some((states.len() - 1, *state.time, *state.arrow_pos, *state.arrow_vel));

                // Simulate the second part of the shot after arrow separation
                // The end time is the time until arrow separation multiplied by the time span factor
                let start_time = system.get_time();
                let end_time = model.settings.timespan_factor*brace_crossing_time;
                let stop_condition = StopCondition::Time(end_time);

                // Set the arrow mass to zero since the arrow is no longer attached to the string
                system.element_mut::<MassElement>(simulation.mass_element_arrow).set_mass(0.0);

                let mut solver = DynamicSolver::new(&mut system, settings);
                solver.solve(stop_condition, &mut |system, eval| {
                    // Skip the first time step, which is identical to the last timestep of the previous solution phase
                    if system.get_time() > start_time {
                        // Evaluate current bow state, update progress and add state
                        let state = simulation.get_bow_state(system, eval, 0.0);
                        progress = state.time/end_time;
                        states.push(state);
                    }

                    return callback(SimulationMode::Dynamic, 100.0*progress);
                }).map_err(ModelError::SimulationDynamicSolutionFailed)?;

                // Compute dissipated damping energy by numerically integrating the damping power
                let damping_energy_limbs = cumulative_simpson(&states.time, &states.damping_power_limbs);
                let damping_energy_string = cumulative_simpson(&states.time, &states.damping_power_string);

                states.damping_energy_limbs = damping_energy_limbs;
                states.damping_energy_string = damping_energy_string;

                // Compute additional dynamic output values

                let arrow_departure = simulation.arrow_departure.map(|(index, _, _, _)| {
                    ArrowDeparture {
                        state_idx: index,
                        arrow_pos: states.arrow_pos[index],
                        arrow_vel: states.arrow_vel[index],
                        kinetic_energy_arrow: states.kinetic_energy_arrow[index],
                        elastic_energy_limbs: states.elastic_energy_limbs[index],
                        kinetic_energy_limbs: states.kinetic_energy_limbs[index],
                        elastic_energy_string: states.elastic_energy_string[index],
                        kinetic_energy_string: states.kinetic_energy_string[index],
                        energy_efficiency: states.kinetic_energy_arrow[index]/statics.final_drawing_work,
                    }
                });

                let max_string_force = discrete_maximum_1d(&states.string_force);
                let max_strand_force = (max_string_force.0/(model.string.n_strands as f64), max_string_force.1);
                let max_draw_force = discrete_maximum_1d(&states.draw_force);
                let min_grip_force = discrete_minimum_1d(&states.grip_force);
                let max_grip_force = discrete_maximum_1d(&states.grip_force);

                let min_layer_stresses = (0..model.section.layers.len()).map(|i_layer| find_min_layer_result(&states.layer_stress, i_layer)).collect();
                let max_layer_stresses = (0..model.section.layers.len()).map(|i_layer| find_max_layer_result(&states.layer_stress, i_layer)).collect();

                // Collect dynamic outputs
                Some(Dynamics {
                    states,
                    arrow_departure,
                    max_string_force,
                    max_strand_force,
                    max_draw_force,
                    min_grip_force,
                    max_grip_force,
                    min_layer_stresses,
                    max_layer_stresses,
                })
            }
            else {
                None
            }
        };

        Ok(BowResult {
            common,
            statics: Some(statics),
            dynamics,
        })
    }

    pub fn simulate_statics(model: &'a BowModel) -> Result<BowResult, ModelError> {
        Self::simulate(model, SimulationMode::Static, |_, _| true)
    }

    pub fn simulate_dynamics(model: &'a BowModel) -> Result<BowResult, ModelError> {
        Self::simulate(model, SimulationMode::Dynamic, |_, _| true)
    }

    pub fn simulate_limb_modes(model: &'a BowModel) -> Result<(Common, Vec<Mode>), ModelError> {
        let (mut system, _simulaion, common) = Self::initialize(model, false, true)?;
        let modes = natural_frequencies(&mut system).map_err(ModelError::SimulationEigenSolutionFailed)?;
        Ok((common, modes))
    }

    // Simulates a static load (two forces, one moment) applied to the limb tip like a cantilever.
    // This is only used for testing the bow bow against other simulations/results.
    pub fn simulate_static_limb(model: &'a BowModel, Fx: f64, Fy: f64, Mz: f64) -> Result<(Common, State), ModelError> {
        let (mut system, simulation, common) = Self::initialize(model, false, false)?;

        if let Some(node) = simulation.limb_nodes.last() {
            system.add_force(node.x(), move |_t| { Fx });
            system.add_force(node.y(), move |_t| { Fy });
            system.add_force(node.φ(), move |_t| { Mz });
        }

        let mut solver = StaticSolver::new(&mut system, newton::NewtonSettings::default());
        let mut states = StateVec::new();

        solver.equilibrium_path_load_controlled(model.settings.min_draw_resolution, &mut |system, eval| {
            let state = simulation.get_bow_state(system, eval, 0.0);
            states.push(state);
            return true;
        }).map_err(ModelError::SimulationStaticSolutionFailed)?;

        Ok((common, states.pop().unwrap()))
    }

    // TODO: Let mutation, write functions for intermediate results
    // TODO: Find a better way to get the stiffness of the force draw curve in there
    fn get_bow_state(&self, system: &System, eval: &SystemEval, draw_stiffness: f64) -> State {
        let time = system.get_time();
        let draw_length = -system.get_displacement(self.string_nodes[0].y());
        let draw_force = -2.0*eval.get_external_force(self.string_nodes[0].y());

        // The evaluation of the arrow position, velocity and acceleration depends on whether the arrow has separated from the string.
        // If the arrow is still attached, the data of the node at the string center is used.
        // If the arrow is separated, its motion is calculated from the velocity at separation.
        let (arrow_acc, arrow_vel, arrow_pos) = if let Some((_, t0, s0, v0)) = self.arrow_departure {
            (
                0.0,                   // Acceleration is zero since no forces act on the arrow anymore
                v0,                    // Velocity is constant since acceleration is zero
                s0 + v0*(time - t0)    // Position develops according to initial position and velocity
            )
        }
        else {
            (
                eval.get_acceleration(self.string_nodes[0].y()),
                system.get_velocity(self.string_nodes[0].y()),
                system.get_displacement(self.string_nodes[0].y()),
            )
        };

        // String kinematics

        let string_pos = system.element_ref::<StringElement>(self.string_element).contact_positions().collect_vec();
        let string_vel = system.element_ref::<StringElement>(self.string_element).contact_velocities().collect_vec();

        // Evaluate positions, velocities, forces and strains at the limb's evaluation points

        let mut limb_pos = Vec::<SVector<f64, 3>>::new();  // TODO: Capacity
        let mut limb_vel = Vec::<SVector<f64, 3>>::new();  // TODO: Capacity
        let mut limb_strain = Vec::<SVector<f64, 3>>::new();  // TODO: Capacity
        let mut limb_force  = Vec::<SVector<f64, 3>>::new();  // TODO: Capacity

        for &element in &self.limb_elements {
            let element = system.element_ref::<BeamElement>(element);
            element.eval_positions().for_each(|u| limb_pos.push(u));
            element.eval_velocities().for_each(|v| limb_vel.push(v));
            element.eval_strains().for_each(|e| limb_strain.push(e));
            element.eval_forces().for_each(|f| limb_force.push(f));
        }

        let mut layer_strain = vec![Vec::<[f64; 2]>::new(); self.input.section.layers.len()];  // TODO: Capacity
        let mut layer_stress = vec![Vec::<[f64; 2]>::new(); self.input.section.layers.len()];  // TODO: Capacity

        for i in 0..limb_strain.len() {
            // Stresses and strains at the layer boundaries
            let strain = &self.geometry.strain_eval[i]*limb_strain[i];
            let stress = &self.geometry.stress_eval[i]*limb_strain[i];

            // Two subsequent strain results make up the belly and back strain of a layer
            strain.iter().cloned().tuples().enumerate().for_each(|(j, tuple): (usize, (f64, f64))| {
                layer_strain[j].push([tuple.0, tuple.1]);
            });

            // Two subsequent stress results make up the belly and back stress of a layer
            stress.iter().cloned().tuples().enumerate().for_each(|(j, tuple): (usize, (f64, f64))| {
                layer_stress[j].push([tuple.0, tuple.1]);
            });
        }

        // The grip force is the y component of the forces at the start of the limb.
        // Defined to be positive on "pressure", therefore the minus sign, and multiplied by two for symmetry.
        let grip_force = -2.0*(limb_force[0][2]*f64::cos(limb_pos[0][2]) + limb_force[0][0]*f64::sin(limb_pos[0][2]));

        let elastic_energy_limbs = 2.0*self.limb_elements.iter().map(|&e| { system.element_ref::<BeamElement>(e).potential_energy() }).sum::<f64>();
        let elastic_energy_string = 2.0*system.element_ref::<StringElement>(self.string_element).potential_energy();

        // The kinetic energy of a single limb is sum of the kinetic energies of the limb elements plus the energy of the limb tip mass.
        // The total kinetic energy of the bow is twice that because of symmetry.
        let kinetic_energy_limbs = 2.0*(self.limb_elements.iter().map(|&e| system.element_ref::<BeamElement>(e).kinetic_energy()).sum::<f64>() + system.element_ref::<MassElement>(self.mass_element_limb_tip).kinetic_energy());
        let kinetic_energy_string = 2.0*(system.element_ref::<MassElement>(self.mass_element_string_center).kinetic_energy() + system.element_ref::<MassElement>(self.mass_element_string_tip).kinetic_energy());
        let kinetic_energy_arrow = 0.5*self.input.masses.arrow*arrow_vel.powi(2);    // Don't use the arrow mass element here

        let damping_power_limbs = 2.0*self.limb_elements.iter().map(|&e| system.element_ref::<BeamElement>(e).dissipative_power()).sum::<f64>();
        let damping_power_string = 2.0*system.element_ref::<StringElement>(self.string_element).dissipative_power();

        // Other string quantities (length, force, angles)

        let string_length = system.element_ref::<StringElement>(self.string_element).get_current_length();
        let string_force = system.element_ref::<StringElement>(self.string_element).normal_force_total();
        let strand_force = string_force/(self.input.string.n_strands as f64);

        let dir_limb_tip: SVector<f64, 2> = (limb_pos[limb_pos.len() - 1] - limb_pos[limb_pos.len() - 2]).fixed_rows::<2>(0).into();
        let dir_string_tip: SVector<f64, 2> = string_pos[string_pos.len() - 1] - string_pos[string_pos.len() - 2];
        let string_tip_angle = dir_limb_tip.angle(&dir_string_tip);

        let dir_string_center = string_pos[1] - string_pos[0];
        let string_center_angle = 2.0*f64::atan2(dir_string_center[0], dir_string_center[1]);

        State {
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

            arrow_pos,
            arrow_vel,
            arrow_acc,

            elastic_energy_limbs,
            elastic_energy_string,

            kinetic_energy_limbs,
            kinetic_energy_string,
            kinetic_energy_arrow,

            damping_energy_limbs: 0.0,    // Integrated from the damping power in a post-processing step
            damping_energy_string: 0.0,    // Integrated from the damping power in a post-processing step
            damping_power_limbs,
            damping_power_string,

            draw_force,
            draw_stiffness,
            grip_force,
            string_length,
            string_tip_angle,
            string_center_angle,
            string_force,
            strand_force,
        }
    }
}

// Input dimensions: (state, layer, length, belly/back)
// Output: (value, [layer, length, belly/back])
fn find_max_layer_result(input: &[Vec<Vec<[f64; 2]>>], i_layer: usize) -> (f64, [usize; 3]) {
    let n_states = input.len();
    let n_length = input[0][0].len();

    discrete_maximum_nd(&mut |i| input[i[0]][i_layer][i[1]][i[2]], [n_states, n_length, 2])
}

// Input dimensions: (state, layer, length, belly/back)
// Output: (value, [layer, length, belly/back])
fn find_min_layer_result(input: &[Vec<Vec<[f64; 2]>>], i_layer: usize) -> (f64, [usize; 3]) {
    let n_states = input.len();
    let n_length = input[0][0].len();

    discrete_minimum_nd(&mut |i| input[i[0]][i_layer][i[1]][i[2]], [n_states, n_length, 2])
}