use nalgebra::{SVector};
use serde::{Deserialize, Serialize};
use soa_derive::StructOfArray;
use virtualbow_num::utils::minmax::{discrete_maximum_1d, discrete_minimum_1d};

use crate::simulation::{find_max_layer_result, find_min_layer_result};

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct BowResult {
    pub common: Common,
    pub statics: Option<Statics>,
    pub dynamics: Option<Dynamics>,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct Common {
    pub limb: LimbInfo,
    pub layers: Vec<LayerInfo>,

    pub string_length: f64,
    pub string_stiffness: f64,
    pub string_mass: f64,
    pub limb_mass: f64,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct Statics {
    pub states: StateVec,

    pub final_draw_force: f64,
    pub final_drawing_work: f64,
    pub storage_factor: f64,

    pub max_forces: MaxForces,
    pub max_stresses: MaxStresses,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct Dynamics {
    pub states: StateVec,

    pub arrow_departure: Option<ArrowDeparture>,
    pub max_forces: MaxForces,
    pub max_stresses: MaxStresses,
}

// Data that is available only if the arrow has separated from the string during the dynamic analysis
#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct ArrowDeparture {
    // Index of the state at which the arrow separated
    pub state_idx: usize,

    // Position and velocity of the arrow at separation
    pub arrow_pos: f64,
    pub arrow_vel: f64,

    // Energies of the components at separation
    pub elastic_energy_limbs: f64,
    pub elastic_energy_string: f64,
    pub kinetic_energy_limbs: f64,
    pub kinetic_energy_string: f64,
    pub kinetic_energy_arrow: f64,
    pub damping_energy_limbs: f64,
    pub damping_energy_string: f64,

    // Degree of efficiency
    pub energy_efficiency: f64,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct MaxForces {
    pub max_string_force: (f64, usize),    // (value, state)
    pub max_strand_force: (f64, usize),    // (value, state)
    pub max_draw_force: (f64, usize),      // (value, state)
    pub min_grip_force: (f64, usize),      // (value, state)
    pub max_grip_force: (f64, usize),      // (value, state)
}

impl MaxForces {
    pub fn from_states(states: &StateVec) -> Self {
        Self {
            max_string_force: discrete_maximum_1d(&states.string_force),
            max_strand_force: discrete_maximum_1d(&states.strand_force),
            max_draw_force: discrete_maximum_1d(&states.draw_force),
            min_grip_force: discrete_minimum_1d(&states.grip_force),
            max_grip_force: discrete_maximum_1d(&states.grip_force),
        }
    }
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct MaxStresses {
    pub max_layer_stress_tension: Vec<(f64, [usize; 3])>,        // (value, [state, length, belly/back]) for each layer
    pub max_layer_stress_compression: Vec<(f64, [usize; 3])>,    // (value, [state, length, belly/back]) for each layer
    pub max_layer_strain_tension: Vec<(f64, [usize; 3])>,        // (value, [state, length, belly/back]) for each layer
    pub max_layer_strain_compression: Vec<(f64, [usize; 3])>,    // (value, [state, length, belly/back]) for each layer
}

impl MaxStresses {
    pub fn from_states(states: &StateVec) -> Self {
        // Only the maximum stress value can be the maximum tension, but only if positive.
        // Negative maximum stress means there is no tension -> 0
        let max_to_tension = |(value, location)| {
            (f64::max(value, 0.0), location)
        };

        // Only the minimum stress value can be the maximum compression, but only if negative.
        // Positive minimum stress means there is no compression -> 0
        let min_to_compression = |(value, location)| {
            (-f64::min(value, 0.0), location)
        };

        // Find minimum and maximum stresses and strains and map to tension and compression using the functions defined above.
        let num_layers = states.layer_stress[0].len();
        Self {
            max_layer_stress_tension: (0..num_layers).map(|i_layer| find_max_layer_result(&states.layer_stress, i_layer)).map(&max_to_tension).collect(),
            max_layer_stress_compression: (0..num_layers).map(|i_layer| find_min_layer_result(&states.layer_stress, i_layer)).map(&min_to_compression).collect(),
            max_layer_strain_tension: (0..num_layers).map(|i_layer| find_max_layer_result(&states.layer_strain, i_layer)).map(&max_to_tension).collect(),
            max_layer_strain_compression: (0..num_layers).map(|i_layer| find_min_layer_result(&states.layer_strain, i_layer)).map(&min_to_compression).collect(),
        }
    }
}

#[derive(StructOfArray, Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
#[soa_derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct State {
    pub time: f64,
    pub draw_length: f64,

    pub limb_pos: Vec<SVector<f64, 3>>,    // x, y, φ
    pub limb_vel: Vec<SVector<f64, 3>>,    // x, y, φ

    pub string_pos: Vec<SVector<f64, 2>>,    // x, y
    pub string_vel: Vec<SVector<f64, 2>>,    // x, y

    pub limb_strain: Vec<SVector<f64, 3>>,    // epsilon, kappa, gamma
    pub limb_force: Vec<SVector<f64, 3>>,     // N, Q, M

    pub layer_strain: Vec<Vec<[f64; 2]>>,     // layer, length, belly/back
    pub layer_stress: Vec<Vec<[f64; 2]>>,     // layer, length, belly/back

    pub arrow_pos: f64,
    pub arrow_vel: f64,
    pub arrow_acc: f64,

    pub elastic_energy_limbs: f64,
    pub elastic_energy_string: f64,

    pub kinetic_energy_limbs: f64,
    pub kinetic_energy_string: f64,
    pub kinetic_energy_arrow: f64,

    pub damping_energy_limbs: f64,
    pub damping_energy_string: f64,
    pub damping_power_limbs: f64,
    pub damping_power_string: f64,

    pub draw_force: f64,
    pub draw_stiffness: f64,
    pub grip_force: f64,
    pub string_length: f64,
    pub string_tip_angle: f64,
    pub string_center_angle: f64,
    pub string_force: f64,
    pub strand_force: f64,
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct LimbInfo {
    // Geometry information needed by the result viewer
    pub length: Vec<f64>,
    pub width: Vec<f64>,
    pub height: Vec<f64>,
    pub bounds: Vec<Vec<f64>>,    // Layer boundaries in y position

    // Geometry information needed by the model editor
    pub ratio: Vec<f64>,
    pub heights: Vec<Vec<f64>>,

    pub position_eval: Vec<SVector<f64, 3>>,       // Eval points of the profile curve (x, y, φ)
    pub position_control: Vec<SVector<f64, 3>>,    // Control points of the profile curve (x, y, φ)

    // TODO: Unify the information
}

#[derive(Serialize, Deserialize, PartialEq, Debug, Clone)]
pub struct LayerInfo {
    pub name: String,
    pub color: String,

    pub maximum_stresses: (f64, f64),
    pub allowed_stresses: (f64, f64),
    pub maximum_strains: (f64, f64),
    pub allowed_strains: (f64, f64),
}