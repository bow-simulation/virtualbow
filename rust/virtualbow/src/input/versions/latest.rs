// Bow file version 4
// Used in VirtualBow input 0.10.*

use itertools::Itertools;
use serde::{Deserialize, Serialize};
use super::{version1, version3};

pub use version1::Width;
pub use version1::Height;
pub use version1::BowString;
pub use version1::Masses;
pub use version1::Damping;

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct BowModel {
    pub comment: String,
    pub settings: Settings,
    pub dimensions: Dimensions,
    pub profile: Profile,
    pub section: Section,
    pub string: BowString,
    pub masses: Masses,
    pub damping: Damping,
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Settings {
    pub num_limb_elements: usize,
    pub num_limb_eval_points: usize,
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

impl Default for Settings {
    fn default() -> Self {
        Self {
            num_limb_elements: 30,
            num_limb_eval_points: 100,
            min_draw_resolution: 100,
            max_draw_resolution: 100,
            arrow_clamp_force: 0.5,
            string_compression_factor: 1e-6,
            timespan_factor: 1.5,
            timeout_factor: 10.0,
            min_timestep: 1e-6,
            max_timestep: 1e-4,
            steps_per_period: 250,
        }
    }
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Dimensions {
    pub brace_height: f64,
    pub draw_length: f64,
    pub handle_reference: HandleReference,
    pub handle_angle: f64,
    pub handle_length: f64,
    pub handle_offset: f64,
}

// Point at the limb root from which the handle's pivot point is measured
#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
#[serde(rename_all = "snake_case")]
pub enum HandleReference {
    Back,
    #[default]
    Belly,
    Profile,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Section {
    pub alignment: LayerAlignment,
    pub width: Width,
    pub materials: Vec<Material>,
    pub layers: Vec<Layer>,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Material {
    pub name: String,
    pub color: String,
    pub density: f64,
    pub youngs_modulus: f64,
    pub shear_modulus: f64
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Layer {
    pub name: String,
    pub material: String,
    pub height: Height,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Profile {
    pub segments: Vec<ProfileSegment>
}

// Defines how the cross sections are aligned with the profile curve
// There are two categories:
// - Section: The profile curve is aligned with the back side, belly side, or geometrical center of the combined section
// - Layer: The profile curve is aligned with the back side, belly side, or geometrical center of a specific layer
#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
#[serde(tag = "type", content = "layer", rename_all = "snake_case")]
pub enum LayerAlignment {
    #[default]
    SectionBack,
    SectionBelly,
    SectionCenter,
    LayerBack(String),
    LayerBelly(String),
    LayerCenter(String)
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
#[serde(tag = "type", content = "parameters", rename_all = "snake_case")]
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

impl From<version3::BowModel> for BowModel {
    fn from(model: version3::BowModel) -> BowModel {
        let settings = Settings {
            num_limb_elements: model.settings.n_limb_elements,
            num_limb_eval_points: 100,
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
            handle_reference: HandleReference::Back,    // Field was newly introduced. Previously the handle was defined with respect to the back of the limb.
            handle_angle: model.dimensions.handle_angle,
            handle_length: model.dimensions.handle_length,
            handle_offset: model.dimensions.handle_setback,
        };

        let materials = model.materials.iter().map(|material| Material {
            name: material.name.clone(),
            color: material.color.clone(),
            density: material.rho,
            youngs_modulus: material.E,
            shear_modulus: material.E/(2.0*(1.0 + 0.4)),  // Shear modulus was newly added. Estimate for poisson ratio v = 0.4.
        }).collect_vec();

        let layers = model.layers.iter().map(|layer| Layer {
            name: layer.name.clone(),
            material: materials[layer.material].name.clone(),
            height: layer.height.clone(),
        }).rev().collect_vec();    // Layers were previously defined from back to belly, but are now from belly to back (direction of the y axis), so the old layers have to be reversed

        let section = Section {
            alignment: LayerAlignment::SectionBack,  // Field was newly introduced. Previously the profile curve was always aligned with the cross section's back.
            width: model.width,
            materials,
            layers,
        };

        let segments = model.profile.iter().map(|segment|{
            match segment {
                version3::ProfileSegment::Line{ parameters } => ProfileSegment::Line(Line{ length: parameters.length }),
                version3::ProfileSegment::Arc{ parameters } => ProfileSegment::Arc(Arc{ length: parameters.length, radius: parameters.radius }),
                version3::ProfileSegment::Spiral{ parameters } => ProfileSegment::Spiral(Spiral{ length: parameters.length, radius_start: parameters.r_start, radius_end: parameters.r_end }),
                version3::ProfileSegment::Spline{ parameters } => ProfileSegment::Spline(Spline{ points: parameters.points.clone() })
            }
        }).collect_vec();

        let profile = Profile {
            segments,
        };

        Self {
            comment: model.comment,
            settings,
            dimensions,
            profile,
            section,
            string: model.string,
            masses: model.masses,
            damping: model.damping,
        }
    }
}