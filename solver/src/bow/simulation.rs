use std::f64::consts::PI;
use clap::ValueEnum;
use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::{SVector, vector};
use crate::fem::elements::bar::BarElement;
use crate::fem::elements::beam::{BeamElementCoRot, CrossSection, PlanarCurve};
use crate::fem::solvers::eigen::{ModeInfo, natural_frequencies};
use crate::fem::solvers::statics::{Settings, StaticSolver};
use crate::fem::system::element::Element;
use crate::fem::system::nodes::{Constraints, GebfNode, PointNode};
use crate::fem::system::system::{StaticEval, System};
use crate::bow::sections::section::LayeredCrossSection;
use crate::bow::errors::ModelError;
use crate::bow::profile::profile::{CurvePoint, ProfileCurve};
use crate::bow::model::BowModel;
use crate::bow::output::{Dynamics, LimbSetup, Output, Setup, State, StateVec, Statics};
use crate::numerics::root_finding::regula_falsi;

#[derive(ValueEnum, Debug, Copy, Clone)]
pub enum SimulationMode {
    Static,
    Dynamic
}

pub struct Simulation<'a> {
    model: &'a BowModel,
    setup: Setup,
    limb_nodes: Vec<GebfNode>,
    limb_elements: Vec<usize>,

    string_node: Option<PointNode>,
    string_element: Option<usize>,
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
    fn new(model: &'a BowModel, string: bool) -> Result<(Simulation, System), ModelError> {
        // Check basic validity of the model data and propagate any errors
        model.validate()?;

        // Profile curve with starting point according to dimension settings
        let start = CurvePoint::new(0.0, model.dimensions.handle_angle, vector![0.5*model.dimensions.handle_length, model.dimensions.handle_setback]);
        let profile = ProfileCurve::new(start, &model.profile.segments)?;

        // Section properties according to layers, materils and alignment to the profile curve
        let section = LayeredCrossSection::new(profile.length(), &model.width, &model.layers, &model.materials, model.profile.alignment)?;

        let s_eval = lin_space(profile.s_start()..=profile.s_end(), model.settings.n_limb_eval_points).collect_vec();           // Lengths at which the limb quantities are evaluated (positions, stress, strain, ...)
        let (segments, _s_nodes, u_nodes) = BeamElementCoRot::discretize(&profile, &section, &s_eval, model.settings.n_limb_elements);
        let elements = segments.iter().map(|segment| BeamElementCoRot::new(segment) );

        let mut system = System::new();

        let limb_nodes: Vec<GebfNode> = u_nodes.iter().enumerate().map(|(i, u)| {
            let constraints = if i != 0 { Constraints::all_free() } else { Constraints::all_fixed() };
            system.create_beam_node(u, constraints)
        }).collect();

        let limb_elements: Vec<usize> = elements.into_iter().enumerate().map(|(i, element)| {
            system.add_element(&[limb_nodes[i], limb_nodes[i+1]], element)
        }).collect();

        // Additional setup data
        let limb_width = s_eval.iter().map(|&s| { section.width(s) }).collect();
        let limb_height = s_eval.iter().map(|&s| { section.height(s) }).collect();
        let limb_density = s_eval.iter().map(|&s| { section.rhoA(s) }).collect();
        let limb_stiffness = s_eval.iter().map(|&s| { section.C(s) }).collect();

        // Only add string node and element if required
        let (string_node, string_element) = if string {
            let x_tip = u_nodes.last().unwrap()[0];
            let y_tip = u_nodes.last().unwrap()[1];
            let y_str = -model.dimensions.brace_height;

            let l = f64::hypot(x_tip, y_str - y_tip);
            let EA = (model.string.n_strands as f64)*model.string.strand_stiffness;
            let rhoA = (model.string.n_strands as f64)*model.string.strand_density;
            let etaA = 4.0*l/PI*f64::sqrt(rhoA*EA)*model.damping.damping_ratio_string;

            let node = system.create_xy_node(0.0, y_str, Constraints::y_pos_free());
            let element = BarElement::new(rhoA, etaA, EA, l);
            let element = system.add_element(&[limb_nodes.last().unwrap().point(), node], element);

            (Some(node), Some(element))
        }
        else {
            (None, None)
        };

        let setup = Setup {
            limb: LimbSetup {
                layers: vec![],    // TODO
                length: s_eval,
                position: vec![],    // TODO
                width: limb_width,
                height: limb_height,
                density: limb_density,
                stiffness: limb_stiffness,
            },
        };

        let simulation = Self {
            model,
            setup,
            limb_nodes,
            limb_elements,
            string_node,
            string_element,
        };

        Ok((simulation, system))
    }

    // Callback: (phase, progress) -> continue
    pub fn simulate<F>(model: &'a BowModel, mode: SimulationMode, mut callback: F) -> Result<Output, ModelError>
        where F: FnMut(&str, f64) -> bool
    {
        let (simulation, mut system) = Self::new(&model, true)?;

        let l0 = system.element_ref::<BarElement>(simulation.string_element.unwrap()).get_initial_length();
        system.add_force(simulation.string_node.unwrap().y(), move |_t| { -1.0 });

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
            system.element_mut::<BarElement>(simulation.string_element.unwrap()).set_initial_length(factor*l0);

            let mut solver = StaticSolver::new(&mut system, Settings::default());
            let result = solver.solve_equilibrium_displacement_controlled(simulation.string_node.unwrap().y(), -model.dimensions.brace_height);
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
                if slope2  <= 0.0 {
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
                    delta *= (Self::BRACING_TARGET_ITER as f64)/(info.iterations as f64);
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

        // "Draw" the bow by solving for a static equilibrium path of the string node from brace height to full draw
        // and store each intermediate step in the static output.
        let mut states = StateVec::new();
        let mut solver = StaticSolver::new(&mut system, Settings::default());

        solver.equilibrium_path_displacement_controlled(simulation.string_node.unwrap().y(), -model.dimensions.draw_length, model.settings.n_draw_steps, &mut |system, statics| {
            let state = simulation.get_bow_state(&system, &statics);
            let progress = 100.0*(state.draw_length - model.dimensions.brace_height)/(model.dimensions.draw_length - model.dimensions.brace_height);
            states.push(state);
            callback("statics", progress)
        }).map_err(|e| ModelError::SimulationStaticSolutionFailed(e))?;

        // Compute additional static output values

        let draw_length_front = *states.draw_length.first().unwrap();
        let draw_length_back = *states.draw_length.last().unwrap();
        let draw_force_back = *states.draw_force.last().unwrap();
        let e_pot_front = states.e_pot_limbs.first().unwrap() + states.e_pot_string.first().unwrap();
        let e_pot_back = states.e_pot_limbs.last().unwrap() + states.e_pot_string.last().unwrap();

        let final_draw_force = draw_force_back;
        let drawing_work = e_pot_back - e_pot_front;
        let storage_factor = (e_pot_back - e_pot_front)/(0.5*(draw_length_back - draw_length_front)*draw_force_back);

        let max_string_force = states.string_force.iter().enumerate().map(|(a, b)| {(*b, a)}).max_by(|(_, a), (_, b)| a.partial_cmp(b).unwrap()).unwrap();  // TODO: Write function for this
        let max_grip_force = states.grip_force.iter().enumerate().map(|(a, b)| {(*b, a)}).max_by(|(_, a), (_, b)| a.partial_cmp(b).unwrap()).unwrap();      // TODO: Write function for this
        let max_draw_force = states.draw_force.iter().enumerate().map(|(a, b)| {(*b, a)}).max_by(|(_, a), (_, b)| a.partial_cmp(b).unwrap()).unwrap();      // TODO: Write function for this

        // Collect static outputs
        let statics = Some(Statics {
            states,
            final_draw_force,
            drawing_work,
            storage_factor,
            max_string_force,
            max_grip_force,
            max_draw_force,
            min_layer_stresses: vec![],
            max_layer_stresses: vec![],
        });

        // TODO: Implement dynamic simulation
        let dynamics = match mode {
            SimulationMode::Static => None,
            SimulationMode::Dynamic => Some(Dynamics::default())
        };

        Ok(Output {
            setup: simulation.setup,
            statics,
            dynamics,
        })
    }

    pub fn simulate_statics(model: &'a BowModel) -> Result<Output, ModelError> {
        Self::simulate(model, SimulationMode::Static, |_, _| true)
    }

    pub fn simulate_dynamics(model: &'a BowModel) -> Result<Output, ModelError> {
        Self::simulate(model, SimulationMode::Dynamic, |_, _| true)
    }

    pub fn simulate_natural_frequencies(model: &'a BowModel) -> Result<(Setup, Vec<ModeInfo>), ModelError> {
        let (simulation, mut system) = Self::new(&model, false)?;
        let results = natural_frequencies(&mut system);
        Ok((simulation.setup, results))
    }

    // Simulates a static load (two forces, one moment) applied to the limb tip like a cantilever.
    // This is only used for testing the bow bow against other simulations/results.
    pub fn simulate_static_load(model: &'a BowModel, Fx: f64, Fy: f64, Mz: f64) -> Result<(Setup, State), ModelError> {
        let (simulation, mut system) = Self::new(&model, false)?;

        if let Some(node) = simulation.limb_nodes.last() {
            system.add_force(node.x(), move |_t| { Fx });
            system.add_force(node.y(), move |_t| { Fy });
            system.add_force(node.φ(), move |_t| { Mz });
        }

        let mut solver = StaticSolver::new(&mut system, Settings::default());
        for lambda in lin_space(0.0..=1.0, model.settings.n_draw_steps) {
            solver.solve_equilibrium_load_controlled(lambda).expect("Static solver failed");
        }

        let statics = solver.statics.clone();  // Only for the borrow checker
        let state = simulation.get_bow_state(&system, &statics);

        Ok((simulation.setup, state))
    }

    // TODO: Lett mutation, write functions for intermediate results
    fn get_bow_state(&self, system: &System, statics: &StaticEval) -> State {
        let time = system.get_time();
        let draw_length = self.string_node.map(|node| { -system.get_displacement(node.y()) }).unwrap_or(0.0);
        let draw_force = self.string_node.map(|node| { -2.0*statics.get_scaled_external_force(node.y()) }).unwrap_or(0.0);

        let string_element = self.string_element.map(|e| { system.element_ref::<BarElement>(e) });
        let string_force = string_element.map(BarElement::normal_force).unwrap_or(0.0);
        let strand_force = string_force/(self.model.string.n_strands as f64);

        // String kinematics

        let limb_tip = self.limb_nodes.last().unwrap();

        let string_pos = self.string_node.map(|node| vec![
            vector![ system.get_displacement(limb_tip.x()), system.get_displacement(limb_tip.y()) ],
            vector![ system.get_displacement(node.x()), system.get_displacement(node.y()) ],
        ]).unwrap_or_default();

        let vel_string = self.string_node.map(|node| vec![
            vector![ system.get_velocity(limb_tip.x()), system.get_velocity(limb_tip.y()) ],
            vector![ system.get_velocity(node.x()), system.get_velocity(node.y()) ],
        ]).unwrap_or_default();

        /*
        let acc_string = self.string_node.map(|node| vec![
            [ system.get_acceleration(limb_tip.x()).unwrap_or(0.0), system.get_acceleration(limb_tip.y()).unwrap_or(0.0) ],
            [ system.get_acceleration(node.x()).unwrap_or(0.0), system.get_acceleration(node.y()).unwrap_or(0.0) ],
        ]).unwrap_or_default();
        */

        // Evaluate positions, forces and strains at the limb's evaluation points

        let mut limb_pos = Vec::new();  // TODO: Capacity
        let mut limb_strain = Vec::<SVector<f64, 3>>::new();  // TODO: Capacity
        let mut limb_force = Vec::<SVector<f64, 3>>::new();  // TODO: Capacity

        for &element in &self.limb_elements {
            let element = system.element_ref::<BeamElementCoRot>(element);
            element.eval_positions().for_each(|u| limb_pos.push(u));
            element.eval_strains().for_each(|e| limb_strain.push(e));
            element.eval_forces().for_each(|f| limb_force.push(f));
        }

        // The grip force is the y component of the forces at the start of the limb.
        // Defined to be positive on "pressure", therefore the minus sign, and multiplied by two for symmetry.
        let grip_force = -2.0*(limb_force[0][2]*f64::cos(limb_pos[0][2]) + limb_force[0][0]*f64::sin(limb_pos[0][2]));

        let e_pot_limbs = 2.0*self.limb_elements.iter().map(|&e| {
            system.element_ref::<BeamElementCoRot>(e).potential_energy()
        }).sum::<f64>();

        let e_kin_limbs = 2.0*self.limb_elements.iter().map(|&e| {
            system.element_ref::<BeamElementCoRot>(e).kinetic_energy()
        }).sum::<f64>();

        let e_pot_string = self.string_element.map(|e| 2.0*system.element_ref::<BarElement>(e).potential_energy()).unwrap_or(0.0);
        let e_kin_string = self.string_element.map(|e| 2.0*system.element_ref::<BarElement>(e).kinetic_energy()).unwrap_or(0.0);

        State {
            time,
            draw_length,

            limb_pos: limb_pos,
            limb_vel: vec![],
            limb_acc: vec![],

            string_pos: string_pos,
            string_vel: vel_string,
            string_acc: vec![],

            limb_strain: limb_strain,
            limb_force: limb_force,

            layer_strain: vec![vec![]],
            layer_stress: vec![vec![]],

            arrow_pos: 0.0,
            arrow_vel: 0.0,
            arrow_acc: 0.0,

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
        let y_str = system.get_displacement(self.string_node.unwrap().y());
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