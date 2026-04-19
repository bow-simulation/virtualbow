// Bow file version 1
// Used in VirtualBow input 0.7.*

use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct BowModel {
    pub comment: String,
    pub settings: Settings,
    pub dimensions: Dimensions,
    pub layers: Vec<Layer>,
    pub profile: Profile,
    pub width: Width,
    pub string: BowString,
    pub masses: Masses,
    pub damping: Damping,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Profile(pub Vec<[f64; 2]>);

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Width(pub Vec<[f64; 2]>);

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Height(pub Vec<[f64; 2]>);

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Settings {
    pub n_draw_steps: usize,
    pub n_limb_elements: usize,
    pub n_string_elements: usize,
    pub sampling_rate: f64,
    pub time_span_factor: f64,
    pub time_step_factor: f64
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Dimensions {
    pub brace_height: f64,
    pub draw_length: f64,
    pub handle_angle: f64,
    pub handle_length: f64,
    pub handle_setback: f64,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Layer {
    pub name: String,
    pub height: Height,
    pub rho: f64,
    pub E: f64,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct BowString {
    pub strand_stiffness: f64,
    pub strand_density: f64,
    pub n_strands: usize,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Masses {
    pub arrow: f64,
    pub limb_tip: f64,
    pub string_center: f64,
    pub string_tip: f64,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Damping {
    pub damping_ratio_limbs: f64,
    pub damping_ratio_string: f64,
}