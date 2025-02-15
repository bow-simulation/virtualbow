// Bow file version 2
// Used in VirtualBow input 0.8.*

use serde::{Serialize, Deserialize};
use crate::input::v1;

pub use v1::Dimensions;
pub use v1::Layer;
pub use v1::Profile;
pub use v1::Width;
pub use v1::BowString;
pub use v1::Masses;
pub use v1::Damping;

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
pub struct Settings {
    pub arrow_clamp_force: f64,
    pub n_draw_steps: usize,
    pub n_limb_elements: usize,
    pub n_string_elements: usize,
    pub sampling_rate: f64,
    pub time_span_factor: f64,
    pub time_step_factor: f64
}

impl From<v1::BowModel> for BowModel {
    fn from(model: v1::BowModel) -> BowModel {
        Self {
            comment: model.comment,
            settings: Settings {
                arrow_clamp_force: 0.0,    // Field was newly introduced
                n_draw_steps: model.settings.n_draw_steps,
                n_limb_elements: model.settings.n_limb_elements,
                n_string_elements: model.settings.n_string_elements,
                sampling_rate: model.settings.sampling_rate,
                time_span_factor: model.settings.time_span_factor,
                time_step_factor: model.settings.time_step_factor,
            },
            dimensions: model.dimensions,
            layers: model.layers,
            profile: model.profile,
            width: model.width,
            string: model.string,
            masses: model.masses,
            damping: model.damping,
        }
    }
}