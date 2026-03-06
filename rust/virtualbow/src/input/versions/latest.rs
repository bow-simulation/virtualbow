// Bow file version 4
// Used in VirtualBow input 0.10.*

use itertools::Itertools;
use serde::{Deserialize, Serialize};
use super::{version1, version3};

pub use version1::Width;
pub use version1::Height;
pub use version1::BowString;
pub use version1::Damping;

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct BowModel {
    pub comment: String,
    pub settings: Settings,
    pub handle: Handle,
    pub draw: Draw,
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
    pub static_iteration_tolerance: f64,
    pub arrow_clamp_force: f64,
    pub string_compression_factor: f64,
    pub timespan_factor: f64,
    pub timeout_factor: f64,
    pub min_timestep: f64,
    pub max_timestep: f64,
    pub steps_per_period: usize,
    pub dynamic_iteration_tolerance: f64
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            num_limb_elements: 30,
            num_limb_eval_points: 250,
            min_draw_resolution: 100,
            max_draw_resolution: 100,
            static_iteration_tolerance: 1e-6,
            arrow_clamp_force: 0.5,
            string_compression_factor: 1e-6,
            timespan_factor: 1.5,
            timeout_factor: 10.0,
            min_timestep: 1e-6,
            max_timestep: 1e-4,
            steps_per_period: 250,
            dynamic_iteration_tolerance: 1e-6
        }
    }
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Draw {
    pub brace_height: f64,
    pub draw_length: DrawLength,
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
#[serde(tag = "type", content = "value", rename_all = "snake_case")]
pub enum DrawLength {
    Standard(f64),    // Measured from the handle's belly or pivot point
    Amo(f64)          // Same but with 1.75" offset according to the AMO standard (not named AMO because snake_case turns that into a_m_o)
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
#[serde(tag = "type", content = "value", rename_all = "snake_case")]
pub enum Handle {
    Flexible,
    Rigid(RigidHandle)
}

// Parameters for a rigid handle sections between the bow limbs
#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct RigidHandle {
    pub length: f64,
    pub angle: f64,
    pub pivot: f64
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Section {
    pub alignment: LayerAlignment,
    pub width: Width,
    pub materials: Vec<Material>,
    pub layers: Vec<Layer>,
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Material {
    pub name: String,
    pub color: String,
    pub density: f64,
    pub youngs_modulus: f64,
    pub shear_modulus: f64,
    pub tensile_strength: f64,
    pub compressive_strength: f64,
    pub safety_margin: f64
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Layer {
    pub name: String,
    pub material: String,
    pub height: Height,
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Profile {
    pub segments: Vec<ProfileSegment>
}

// Defines how the cross-sections are aligned with the profile curve
// There are two categories:
// - Section: The profile curve is aligned with the back side, belly side, or geometrical center of the combined section
// - Layer: The profile curve is aligned with the back side, belly side, or geometrical center of a specific layer
#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
#[serde(tag = "type", content = "layer", rename_all = "snake_case")]
pub enum LayerAlignment {
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

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
#[serde(tag = "type", content = "value", rename_all = "snake_case")]
pub enum ArrowMass {
    Mass(f64),
    MassPerForce(f64),
    MassPerEnergy(f64)
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Masses {
    pub arrow: ArrowMass,
    pub limb_tip: f64,
    pub string_center: f64,
    pub string_tip: f64,
}

impl From<version3::BowModel> for BowModel {
    fn from(model: version3::BowModel) -> BowModel {
        let settings = Settings {
            num_limb_elements: model.settings.n_limb_elements,
            min_draw_resolution: model.settings.n_draw_steps,
            max_draw_resolution: model.settings.n_draw_steps,
            arrow_clamp_force: model.settings.arrow_clamp_force,
            timespan_factor: model.settings.time_span_factor,
            .. Default::default()
        };

        let handle = Handle::Rigid(RigidHandle {
            length: model.dimensions.handle_length,
            angle: model.dimensions.handle_angle,
            pivot: model.dimensions.handle_setback,
        });

        // The brace height changed reference point from back to belly,
        // therefore we need the thickness of the limb root and the limb angle to work out the difference.
        let thickness: f64 = model.layers.iter()
            .filter(|layer| layer.height.0[0][0] == 0.0)    // Filter layers that have a thickness at relative position 0
            .map(|layer| layer.height.0[0][1])              // Extract those thicknesses...as
            .sum();                                       // And sum them up

        // The offset for brace height and draw length is the thickness of the limb in the direction of draw
        let offset = thickness*f64::cos(model.dimensions.handle_angle);

        let draw = Draw {
            brace_height: model.dimensions.brace_height - offset,
            draw_length: DrawLength::Standard(model.dimensions.draw_length - offset),
        };

        let materials = model.materials.iter().map(|material| {
            let mut shear_modulus = material.E/(2.0*(1.0 + 0.4));    // Shear modulus was newly added. Estimate for poisson ratio v = 0.4
            let factor = 0.1*material.E;
            shear_modulus = (shear_modulus/factor).round()*factor;   // Round to precision based on E
            shear_modulus = f64::max(shear_modulus, factor);         // Minimum if rounded to zero

            Material {
                name: material.name.clone(),
                color: material.color.clone(),
                density: material.rho,
                youngs_modulus: material.E,
                shear_modulus,
                tensile_strength: 0.0,        // Field was newly introduced, value unknown.
                compressive_strength: 0.0,    // Field was newly introduced, value unknown.
                safety_margin: 0.0            // Field was newly introduced, value unknown.
            }
        }).collect_vec();

        let layers = model.layers.iter().map(|layer| Layer {
            name: layer.name.clone(),
            material: materials[layer.material].name.clone(),
            height: layer.height.clone(),
        }).collect_vec();
        
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

        let masses = Masses {
            arrow: ArrowMass::Mass(model.masses.arrow),
            limb_tip: model.masses.limb_tip,
            string_center: model.masses.string_center,
            string_tip: model.masses.string_tip,
        };

        Self {
            comment: model.comment,
            settings,
            handle,
            draw,
            profile,
            section,
            string: model.string,
            masses,
            damping: model.damping,
        }
    }
}