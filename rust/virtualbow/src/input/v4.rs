// Bow file version 4
// Used in VirtualBow input 0.10.*

use itertools::Itertools;
use serde::{Deserialize, Serialize};
use crate::input::{v1, v3};

pub use v1::Width;
pub use v1::Height;
pub use v1::BowString;
pub use v1::Masses;
pub use v1::Damping;

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct BowModel {
    pub comment: String,
    pub settings: Settings,
    pub dimensions: Dimensions,
    pub materials: Vec<Material>,
    pub layers: Vec<Layer>,
    pub profile: Profile,
    pub width: Width,
    pub string: BowString,
    pub masses: Masses,
    pub damping: Damping,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Settings {
    pub n_limb_elements: usize,
    pub n_limb_eval_points: usize,
    pub min_draw_resolution: usize,
    pub max_draw_resolution: usize,
    pub arrow_clamp_force: f64,
    pub string_compression_factor: f64,
    pub timespan_factor: f64,
    pub timeout_factor: f64,
    pub min_timestep: f64,
    pub max_timestep: f64,
    pub steps_per_period: usize
}

// Point at the limb root from which the handle's pivot point is measured
#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
#[serde(rename_all = "snake_case")]
pub enum HandleReference {
    Back,
    Belly,
    #[default]
    Profile,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Dimensions {
    pub brace_height: f64,
    pub draw_length: f64,
    pub handle_ref: HandleReference,
    pub handle_angle: f64,
    pub handle_length: f64,
    pub handle_offset: f64,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Material {
    pub name: String,
    pub color: String,
    pub rho: f64,
    pub E: f64,
    pub G: f64
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Layer {
    pub name: String,
    pub material: String,
    pub height: Height,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Profile {
    pub alignment: ProfileAlignment,
    pub segments: Vec<ProfileSegment>,
}

// Defines, how the cross sections are aligned with the profile curve
// There are two categories:
// - Section: The back side, belly side, or geometrical center of the combined section is aligned with the profile curve
// - Layer: The back side, belly side, or geometrical center of the layer with the given name is aligned with the profile curve.
#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
#[serde(rename_all = "snake_case")]
pub enum ProfileAlignment {
    #[default]
    SectionBack,
    SectionBelly,
    SectionCenter,
    LayerBack(String),
    LayerBelly(String),
    LayerCenter(String)
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
#[serde(tag = "type", rename_all = "snake_case")]
pub enum ProfileSegment {
    Line(Line),
    Arc(Arc),
    Spiral(Spiral),
    Spline(Spline),
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Line {
    pub length: f64
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Arc {
    pub length: f64,
    pub radius: f64
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Spiral {
    pub length: f64,
    pub radius_start: f64,
    pub radius_end: f64,
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Spline {
    pub points: Vec<[f64; 2]>
}

impl From<v3::BowModel> for BowModel {
    fn from(model: v3::BowModel) -> BowModel {
        let settings = Settings {
            n_limb_elements: model.settings.n_limb_elements,
            n_limb_eval_points: 100,
            min_draw_resolution: model.settings.n_draw_steps,
            max_draw_resolution: model.settings.n_draw_steps,
            arrow_clamp_force: model.settings.arrow_clamp_force,
            string_compression_factor: 1e-6,
            timespan_factor: model.settings.time_span_factor,
            timeout_factor: 5.0,
            min_timestep: 1e-6,
            max_timestep: 1e-4,
            steps_per_period: 250,
        };

        let dimensions = Dimensions {
            brace_height: model.dimensions.brace_height,
            draw_length: model.dimensions.draw_length,
            handle_ref: HandleReference::Profile,    // Field was newly introduced. Previously the handle was defined with respect to the limb's profile curve.
            handle_angle: model.dimensions.handle_angle,
            handle_length: model.dimensions.handle_length,
            handle_offset: model.dimensions.handle_setback,
        };

        let materials = model.materials.iter().map(|material| Material {
            name: material.name.clone(),
            color: material.color.clone(),
            rho: material.rho,
            E: material.E,
            G: material.E/(2.0*(1.0 + 0.4)),  // Shear modulus was newly added. Estimate for poisson ratio v = 0.4.
        }).collect_vec();

        let layers = model.layers.iter().map(|layer| Layer {
            name: layer.name.clone(),
            material: materials[layer.material].name.clone(),
            height: layer.height.clone(),
        }).collect_vec();

        let segments = model.profile.iter().map(|segment|{
            match segment {
                v3::ProfileSegment::Line{ parameters } => ProfileSegment::Line(Line{ length: parameters.length }),
                v3::ProfileSegment::Arc{ parameters } => ProfileSegment::Arc(Arc{ length: parameters.length, radius: parameters.radius }),
                v3::ProfileSegment::Spiral{ parameters } => ProfileSegment::Spiral(Spiral{ length: parameters.length, radius_start: parameters.r_start, radius_end: parameters.r_end }),
                v3::ProfileSegment::Spline{ parameters } => ProfileSegment::Spline(Spline{ points: parameters.points.clone() })
            }
        }).collect_vec();

        let profile = Profile {
            alignment: ProfileAlignment::SectionBack,  // Field was newly introduced. Previously the profile curve was always aligned with the cross section's back.
            segments,
        };

        Self {
            comment: model.comment,
            settings,
            dimensions,
            materials,
            layers,
            profile,
            width: model.width,
            string: model.string,
            masses: model.masses,
            damping: model.damping,
        }
    }
}