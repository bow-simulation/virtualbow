use std::f64::consts::{PI, FRAC_PI_2};
use clap::ValueEnum;
use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::vector;
use soa_rs::Soa;
use crate::fem::elements::bar::BarElement;
use crate::fem::elements::beam::{BeamElement, CrossSection, PlanarCurve};
use crate::fem::solvers::eigen::{Mode, natural_frequencies};
use crate::fem::solvers::statics::StaticSolver;
use crate::fem::system::element::Element;
use crate::fem::system::nodes::{Constraints, OrientedNode, PointNode};
use crate::fem::system::system::{DynamicEval, StaticEval, System};
use crate::bow::sections::section::LayeredCrossSection;
use crate::bow::errors::ModelError;
use crate::bow::profile::profile::{CurvePoint, ProfileCurve};
use crate::bow::input::BowInput;
use crate::bow::output::{Dynamics, LayerInfo, LimbInfo, BowOutput, Common, State, Statics};
use crate::fem::elements::mass::MassElement;
use crate::fem::solvers::{dynamics, statics};
use crate::fem::solvers::dynamics::DynamicSolver;
use crate::numerics::root_finding::regula_falsi;

#[derive(ValueEnum, PartialEq, Debug, Copy, Clone)]
pub enum SimulationMode {
    Static,
    Dynamic
}

pub enum SystemEval<'a> {
    Static(&'a StaticEval),
    Dynamic(&'a DynamicEval),
}

pub struct Simulation<'a> {
    model: &'a BowInput,
    info: Common,
    limb_nodes: Vec<OrientedNode>,
    limb_elements: Vec<usize>,

    string_node: PointNode,
    string_element: usize,

    // Arrow mass element positioned at the string center
    arrow_element: usize,

    // None if the arrow is still attached to the string
    // Otherwise the time, position and velocity at separation from the string
    arrow_separation: Option<(f64, f64, f64)>,
}

impl<'a> Simulation<'a> {
    // Numerical constants for the bracing simulation
    // TODO: Maybe put all those numerical parameters into a separate struct
    const BRACING_DELTA_START: f64 = 1e-3;       // Initial decrement in string length
    const BRACING_DELTA_MIN: f64 = 1e-5;         // Minimum step length, abort if smaller
    const BRACING_SLOPE_TOL: f64 = 1e-6;         // Tolerance for the string's slope error
    const BRACING_MAX_ROOT_ITER: usize = 20;     // Maximum number of iterations for the terminal root finding algorithm
    const BRACING_TARGET_ITER: usize = 5;        // Desired number of iterations for the static solver

    // Set up the simulation either with or without string, depending on simulation mode
    fn new(model: &'a BowInput, string: bool) -> Result<(Simulation, System), ModelError> {
        // Check basic validity of the model data and propagate any errors
        model.validate()?;

        // Profile curve with starting point according to dimension settings
        let start = CurvePoint::new(0.0, model.dimensions.handle_angle, vector![0.5*model.dimensions.handle_length, model.dimensions.handle_setback]);
        let profile = ProfileCurve::new(start, &model.profile.segments)?;

        // Section properties according to layers, materils and alignment to the profile curve
        let section = LayeredCrossSection::new(profile.length(), &model.width, &model.layers, &model.materials, model.profile.alignment)?;

        let s_eval = lin_space(profile.s_start()..=profile.s_end(), model.settings.n_limb_eval_points).collect_vec();           // Lengths at which the limb quantities are evaluated (positions, stress, strain, ...)
        let (segments, _s_nodes, u_nodes) = BeamElement::discretize(&profile, &section, &s_eval, model.settings.n_limb_elements);
        let elements = segments.iter().map(|segment| BeamElement::new(segment) );

        let mut system = System::new();

        let limb_nodes: Vec<OrientedNode> = u_nodes.iter().enumerate().map(|(i, u)| {
            let constraints = if i != 0 { Constraints::all_free() } else { Constraints::all_fixed() };
            system.create_oriented_node(u, constraints)
        }).collect();

        let limb_elements: Vec<usize> = elements.into_iter().enumerate().map(|(i, element)| {
            system.add_element(&[limb_nodes[i], limb_nodes[i+1]], element)
        }).collect();

        // Layer setup data
        let layers = model.layers.iter().map(|layer| {
            let l0 = profile.s_start() + profile.length()*layer.height.first().unwrap()[0];    // Start arc length of the layer
            let l1 = profile.s_start() + profile.length()*layer.height.last().unwrap()[0];     // End arc length of the layer
            LayerInfo {
                name: layer.name.clone(),
                length: lin_space(l0..=l1, model.settings.n_layer_eval_points).collect(),
            }
        }).collect_vec();

        // Additional setup data
        let limb_position = s_eval.iter().map(|&s| {
            let position = profile.position(s);
            [position[0], position[1], profile.angle(s)]
        }).collect();
        let limb_width = s_eval.iter().map(|&s| { section.width(s) }).collect();
        let limb_height = s_eval.iter().map(|&s| { section.height(s) }).collect();
        //let limb_density = s_eval.iter().map(|&s| { section.rhoA(s) }).collect();
        //let limb_stiffness = s_eval.iter().map(|&s| { section.C(s) }).collect();

        // Limb tip and string center positions
        let x_tip = u_nodes.last().unwrap()[0];
        let y_tip = u_nodes.last().unwrap()[1];
        let y_str = -model.dimensions.brace_height;

        // Add string center node and make it fixed in the case of no string
        let string_node = if string {
            system.create_point_node(&vector![0.0, y_str], Constraints::y_pos_free())
        }
        else {
            system.create_point_node(&vector![0.0, y_str], Constraints::all_fixed())
        };

        // The string element only gets non-zero parameters if the string option is true
        let l = f64::hypot(x_tip, y_str - y_tip);
        let EA = if string { (model.string.n_strands as f64)*model.string.strand_stiffness } else { 0.0 };
        let rhoA = if string { (model.string.n_strands as f64)*model.string.strand_density } else { 0.0 };
        let etaA = if string { 4.0*l/PI*f64::sqrt(rhoA*EA)*model.damping.damping_ratio_string } else { 0.0 };
        let string_element = BarElement::new(rhoA, etaA, EA, l);
        let string_element = system.add_element(&[limb_nodes.last().unwrap().point(), string_node], string_element);

        // Add arrow mass and other additional masses
        let arrow_element = system.add_element(&[string_node], MassElement::new(0.5*model.masses.arrow));
        system.add_element(&[string_node], MassElement::new(0.5*model.masses.string_center));
        system.add_element(&[limb_nodes.last().unwrap().point()], MassElement::new(model.masses.limb_tip));
        system.add_element(&[limb_nodes.last().unwrap().point()], MassElement::new(model.masses.string_tip));

        let info = Common {
            limb: LimbInfo {
                length: s_eval,
                position: limb_position,
                width: limb_width,
                height: limb_height,
            },
            layers,
            string_length: 0.0,
            string_mass: 0.0,
            limb_mass: 0.0,
        };

        let simulation = Self {
            model,
            info,
            limb_nodes,
            limb_elements,
            string_node,
            string_element,
            arrow_element,
            arrow_separation: None,
        };

        Ok((simulation, system))
    }

    // Callback: (phase, progress) -> continue
    pub fn simulate<F>(model: &'a BowInput, mode: SimulationMode, mut callback: F) -> Result<BowOutput, ModelError>
        where F: FnMut(&str, f64) -> bool
    {
        let (mut simulation, mut system) = Self::new(&model, true)?;

        let statics = {
            let l0 = system.element_ref::<BarElement>(simulation.string_element).get_initial_length();
            system.add_force(simulation.string_node.y(), move |_t| { -1.0 });

            // Initial values for the string factor, the slope and the step size for iterating on the string factor
            let mut factor1 = 1.0;
            let mut slope1 = simulation.get_string_slope(&system);
            let mut delta = Self::BRACING_DELTA_START;

            // Abort if the initial slope is negative, which means that the supplied brace height is too
            if slope1 < 0.0 {
                // TODO: Determine the minimum required brace height and put it into the error message
                return Err(ModelError::SimulationBraceHeightTooLow(model.dimensions.brace_height));
            }

            // Applies the given string length to the bow, solves for static equilibrium with the string pinned at brace height.
            // Returns the slope of the string as well as the return state of the static solver.
            // The root of this function is the string length that braces the bow with the desired brace height.
            let mut try_string_length = |factor: f64| {
                system.element_mut::<BarElement>(simulation.string_element).set_initial_length(factor * l0);

                let mut solver = StaticSolver::new(&mut system, statics::Settings::default());
                let result = solver.equilibrium_displacement_controlled(simulation.string_node.y(), -model.dimensions.brace_height);
                let slope = simulation.get_string_slope(&system);

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
                        regula_falsi(try_string_length, factor1, factor2, slope1, slope2, 0.0, Self::BRACING_SLOPE_TOL, Self::BRACING_MAX_ROOT_ITER).ok_or(ModelError::SimulationBracingNoConvergence)?;
                        break;
                    }
                    else {
                        // Otherwise apply step and continue
                        factor1 = factor2;
                        slope1 = slope2;

                        // Adjust step size according to static solver performance
                        delta *= (Self::BRACING_TARGET_ITER as f64) / (info.iterations as f64);
                    }
                } else {
                    // Static iteration failure: Reduce step size by a generic factor
                    delta /= 2.0;
                }

                // Abort if the step size becomes too small
                if delta < Self::BRACING_DELTA_MIN {
                    return Err(ModelError::SimulationBracingNoSignChange)
                }
            }

            // "Draw" the bow by solving for a static equilibrium path of the string node from brace height to full draw
            // and store each intermediate step in the static output.
            let mut states = Soa::<State>::new();
            let mut solver = StaticSolver::new(&mut system, statics::Settings::default());

            solver.equilibrium_path_displacement_controlled(simulation.string_node.y(), -model.dimensions.draw_length, model.settings.n_draw_steps, &mut |system, eval| {
                let state = simulation.get_bow_state(&system, SystemEval::Static(&eval));
                let progress = 100.0 * (state.draw_length - model.dimensions.brace_height) / (model.dimensions.draw_length - model.dimensions.brace_height);
                states.push(state);
                callback("statics", progress)
            }).map_err(|e| ModelError::SimulationStaticSolutionFailed(e))?;

            // Compute additional static output values

            let draw_length_front = *states.draw_length().first().unwrap();
            let draw_length_back = *states.draw_length().last().unwrap();
            let draw_force_back = *states.draw_force().last().unwrap();
            let e_pot_front = states.e_pot_limbs().first().unwrap() + states.e_pot_string().first().unwrap();
            let e_pot_back = states.e_pot_limbs().last().unwrap() + states.e_pot_string().last().unwrap();

            let final_draw_force = draw_force_back;
            let final_drawing_work = e_pot_back - e_pot_front;
            let storage_factor = (e_pot_back - e_pot_front) / (0.5 * (draw_length_back - draw_length_front) * draw_force_back);

            let max_string_force = states.string_force().iter().enumerate().map(|(a, b)| { (*b, a) }).max_by(|(_, a), (_, b)| a.partial_cmp(b).unwrap()).unwrap();  // TODO: Write function for this
            let max_strand_force = (max_string_force.0 / (model.string.n_strands as f64), max_string_force.1);
            let max_grip_force = states.grip_force().iter().enumerate().map(|(a, b)| { (*b, a) }).max_by(|(_, a), (_, b)| a.partial_cmp(b).unwrap()).unwrap();      // TODO: Write function for this
            let max_draw_force = states.draw_force().iter().enumerate().map(|(a, b)| { (*b, a) }).max_by(|(_, a), (_, b)| a.partial_cmp(b).unwrap()).unwrap();      // TODO: Write function for this

            // Collect static outputs
            Some(Statics {
                states,
                final_draw_force,
                final_drawing_work,
                storage_factor,
                max_string_force,
                max_strand_force,
                max_grip_force,
                max_draw_force,
                min_layer_stresses: vec![(0.0, 0, 0); model.layers.len()],
                max_layer_stresses: vec![(0.0, 0, 0); model.layers.len()],
            })
        };

        // Perform dynamic simulation, if required
        let dynamics = {
            if mode == SimulationMode::Dynamic {
                let settings = dynamics::Settings { timestep: 1e-4, ..Default::default() };
                let mut states = Soa::<State>::new();

                // Estimate timeout after which to abort the simulation
                let k_bow = statics.as_ref().unwrap().final_draw_force/(model.dimensions.draw_length - model.dimensions.brace_height);
                let t_max = model.settings.timeout_factor*FRAC_PI_2*f64::sqrt(model.masses.arrow/k_bow);

                // Simulate the first part of the shot until either the arrow separates from the string
                // or the timeout is reached for some reason
                let mut solver = DynamicSolver::new(&mut system, settings);
                solver.solve(&mut |system, eval| {
                    // Evaluate current bow state
                    let state = simulation.get_bow_state(&system, SystemEval::Dynamic(&eval));

                    // Check for separation of the arrow (negative acceleration overcomes clamp force)
                    // On separation, remember the time, position and velocity
                    if state.arrow_acc <= -model.settings.arrow_clamp_force/model.masses.arrow {
                        simulation.arrow_separation = Some((state.time, state.arrow_pos, state.arrow_vel));
                    }

                    // Push bow state into the final results
                    states.push(state);

                    // Continue the simulation as long as the arrow is not separated
                    // and the timeout has not yet been reached
                    return simulation.arrow_separation.is_none() && system.get_time() < t_max;
                });

                // Simulate the second part of the shot after arrow separation,
                // but only if separation actually occurred
                if simulation.arrow_separation.is_some() {
                    // Set the arrow mass to zero since the arrow is no longer attached to the string
                    system.element_mut::<MassElement>(simulation.arrow_element).set_mass(0.0);

                    // The end time is the time until arrow separation multiplied by the time span factor
                    let t_end = model.settings.timespan_factor*system.get_time();

                    let mut solver = DynamicSolver::new(&mut system, settings);
                    solver.solve(&mut |system, eval| {
                        // Evaluate and push back current bow state
                        let state = simulation.get_bow_state(&system, SystemEval::Dynamic(&eval));
                        states.push(state);

                        // Continue as long as the end time is not reached
                        return system.get_time() < t_end;
                    });
                }

                Some(Dynamics {
                    states,
                    final_arrow_pos: 0.0,
                    final_arrow_vel: 0.0,
                    final_e_kin_arrow: 0.0,
                    final_e_pot_limbs: 0.0,
                    final_e_kin_limbs: 0.0,
                    final_e_pot_string: 0.0,
                    final_e_kin_string: 0.0,
                    energy_efficiency: 0.0,
                    max_string_force: (0.0, 0),
                    max_strand_force: (0.0, 0),
                    max_grip_force: (0.0, 0),
                    max_draw_force: (0.0, 0),
                    min_layer_stresses: vec![(0.0, 0, 0); model.layers.len()],
                    max_layer_stresses: vec![(0.0, 0, 0); model.layers.len()],
                })
            }
            else {
                None
            }
        };

        Ok(BowOutput {
            common: simulation.info,
            statics,
            dynamics,
        })
    }

    pub fn simulate_statics(model: &'a BowInput) -> Result<BowOutput, ModelError> {
        Self::simulate(model, SimulationMode::Static, |_, _| true)
    }

    pub fn simulate_dynamics(model: &'a BowInput) -> Result<BowOutput, ModelError> {
        Self::simulate(model, SimulationMode::Dynamic, |_, _| true)
    }

    pub fn simulate_natural_frequencies(model: &'a BowInput) -> Result<(Common, Vec<Mode>), ModelError> {
        let (simulation, mut system) = Self::new(&model, false)?;
        let results = natural_frequencies(&mut system);
        Ok((simulation.info, results))
    }

    // Simulates a static load (two forces, one moment) applied to the limb tip like a cantilever.
    // This is only used for testing the bow bow against other simulations/results.
    pub fn simulate_static_load(model: &'a BowInput, Fx: f64, Fy: f64, Mz: f64) -> Result<(Common, State), ModelError> {
        let (simulation, mut system) = Self::new(&model, false)?;

        if let Some(node) = simulation.limb_nodes.last() {
            system.add_force(node.x(), move |_t| { Fx });
            system.add_force(node.y(), move |_t| { Fy });
            system.add_force(node.φ(), move |_t| { Mz });
        }

        let mut solver = StaticSolver::new(&mut system, statics::Settings::default());
        let mut states = Soa::<State>::new();

        solver.equilibrium_path_load_controlled(model.settings.n_draw_steps, &mut |system, eval| {
            let state = simulation.get_bow_state(&system, SystemEval::Static(&eval));
            states.push(state);
            return true;
        }).map_err(|e| ModelError::SimulationStaticSolutionFailed(e))?;

        Ok((simulation.info, states.pop().unwrap()))
    }

    // TODO: Lett mutation, write functions for intermediate results
    fn get_bow_state(&self, system: &System, eval: SystemEval) -> State {
        let time = system.get_time();
        let draw_length = -system.get_displacement(self.string_node.y());
        let draw_force = match eval {
            SystemEval::Static(eval) => -2.0*eval.get_scaled_external_force(self.string_node.y()),
            SystemEval::Dynamic(eval) => -2.0*eval.get_external_force(self.string_node.y())
        };

        let string_force = system.element_ref::<BarElement>(self.string_element).normal_force();
        let strand_force = string_force/(self.model.string.n_strands as f64);

        // The evaluation of the arrow position, velocity and acceleration depends on whether the arrow has separated from the string.
        // If the arrow is still attached, the data of the node at the string center is used.
        // If the arrow is separated, its motion is calculated from the velocity at separation.
        let (arrow_acc, arrow_vel, arrow_pos) = if let Some((t0, s0, v0)) = self.arrow_separation {
            (
                0.0,                   // Acceleration is zero since no forces act on the arrow anymore
                v0,                    // Velocity is constant since acceleration is zero
                s0 + v0*(time - t0)    // Position develops according to initial position and velocity
            )
        }
        else {
            (
                match eval {
                    SystemEval::Static(_) => 0.0,
                    SystemEval::Dynamic(eval) => eval.get_acceleration(self.string_node.y())
                },
                system.get_velocity(self.string_node.y()),
                system.get_displacement(self.string_node.y()),
            )
        };

        // String kinematics

        let limb_tip = self.limb_nodes.last().unwrap();

        let string_pos = vec![
            [ system.get_displacement(limb_tip.x()), system.get_displacement(limb_tip.y()) ],
            [ system.get_displacement(self.string_node.x()), system.get_displacement(self.string_node.y()) ],
        ];

        let string_vel = vec![
            [ system.get_velocity(limb_tip.x()), system.get_velocity(limb_tip.y()) ],
            [ system.get_velocity(self.string_node.x()), system.get_velocity(self.string_node.y()) ],
        ];

        /*
        let acc_string = self.string_node.map(|node| vec![
            [ system.get_acceleration(limb_tip.x()).unwrap_or(0.0), system.get_acceleration(limb_tip.y()).unwrap_or(0.0) ],
            [ system.get_acceleration(node.x()).unwrap_or(0.0), system.get_acceleration(node.y()).unwrap_or(0.0) ],
        ]).unwrap_or_default();
        */

        // Evaluate positions, forces and strains at the limb's evaluation points

        let mut limb_pos    = Vec::<[f64; 3]>::new();  // TODO: Capacity
        let mut limb_strain = Vec::<[f64; 3]>::new();  // TODO: Capacity
        let mut limb_force  = Vec::<[f64; 3]>::new();  // TODO: Capacity

        for &element in &self.limb_elements {
            let element = system.element_ref::<BeamElement>(element);
            element.eval_positions().for_each(|u| limb_pos.push(u.into()));
            element.eval_strains().for_each(|e| limb_strain.push(e.into()));
            element.eval_forces().for_each(|f| limb_force.push(f.into()));
        }

        // The grip force is the y component of the forces at the start of the limb.
        // Defined to be positive on "pressure", therefore the minus sign, and multiplied by two for symmetry.
        let grip_force = -2.0*(limb_force[0][2]*f64::cos(limb_pos[0][2]) + limb_force[0][0]*f64::sin(limb_pos[0][2]));

        let e_pot_limbs = 2.0*self.limb_elements.iter().map(|&e| {
            system.element_ref::<BeamElement>(e).potential_energy()
        }).sum::<f64>();

        let e_kin_limbs = 2.0*self.limb_elements.iter().map(|&e| {
            system.element_ref::<BeamElement>(e).kinetic_energy()
        }).sum::<f64>();

        let e_pot_string = 2.0*system.element_ref::<BarElement>(self.string_element).potential_energy();
        let e_kin_string = 2.0*system.element_ref::<BarElement>(self.string_element).kinetic_energy();

        State {
            time,
            draw_length,

            limb_pos,
            limb_vel: vec![[0.0; 3]; self.model.settings.n_limb_eval_points],
            limb_acc: vec![[0.0; 3]; self.model.settings.n_limb_eval_points],

            string_pos,
            string_vel,
            string_acc: vec![[0.0; 2]; 2],

            limb_strain,
            limb_force,

            layer_strain: vec![vec![(0.0, 0.0); self.model.settings.n_layer_eval_points]; self.model.layers.len()],
            layer_stress: vec![vec![(0.0, 0.0); self.model.settings.n_layer_eval_points]; self.model.layers.len()],

            arrow_pos,
            arrow_vel,
            arrow_acc,

            e_pot_limbs,
            e_kin_limbs,
            e_pot_string,
            e_kin_string,
            e_kin_arrow: 0.0,

            draw_force,
            grip_force,
            string_force,
            strand_force,
        }
    }

    // Returns the slope of the string against the x direction at its center
    fn get_string_slope(&self, system: &System) -> f64 {
        let x_tip = system.get_displacement(self.limb_nodes.last().unwrap().x());
        let y_tip = system.get_displacement(self.limb_nodes.last().unwrap().y());
        let y_str = system.get_displacement(self.string_node.y());
        (y_tip - y_str)/x_tip
    }
}

/*
// Extrema of a function integer -> float
fn discrete_extrema_1<F>(f: F, n: usize)
    where F: FnMut(usize) -> f64
{

}
*/