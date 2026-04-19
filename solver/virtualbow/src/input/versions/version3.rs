// Bow file version 3
// Used in VirtualBow versions 0.9.*

use std::collections::hash_map::Entry;
use std::collections::HashMap;
use std::hash::{DefaultHasher, Hash, Hasher};
use itertools::Itertools;
use serde::{Serialize, Deserialize};
use super::{version1, version2};

pub use version1::Dimensions;
pub use version1::Width;
pub use version1::Height;
pub use version1::BowString;
pub use version1::Masses;
pub use version1::Damping;
pub use version2::Settings;

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct BowModel {
    pub comment: String,
    pub settings: Settings,
    pub dimensions: Dimensions,
    pub materials: Vec<Material>,
    pub layers: Vec<Layer>,
    pub profile: Vec<ProfileSegment>,
    pub width: Width,
    pub string: BowString,
    pub masses: Masses,
    pub damping: Damping,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Material {
    pub name: String,
    pub color: String,
    pub rho: f64,
    pub E: f64,
}

#[derive(Serialize, Deserialize, Default, Debug, Clone, PartialEq)]
pub struct Layer {
    pub name: String,
    pub material: usize,
    pub height: Height,
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
#[serde(tag = "type", rename_all = "kebab-case")]
pub enum ProfileSegment {
    Line{ parameters: Line },
    Arc{ parameters: Arc },
    Spiral{ parameters: Spiral },
    Spline{ parameters: Spline },
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
    pub r_start: f64,
    pub r_end: f64
}

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq)]
pub struct Spline {
    pub points: Vec<[f64; 2]>
}

impl From<version2::BowModel> for BowModel {
    fn from(model: version2::BowModel) -> BowModel {
        // In previous input, the colors were randomly generated based on the material properties (same properties -> same color)
        // Starting with version 0.9 the colors can be chosen by users, so we have to pick some initial color here. Instead of replicating the old
        // random algorithm, they are chosen out of a fixed color palette (taken from Python's Matplotlib,  https://stackoverflow.com/a/42091037)
        const COLOR_PALETTE: &[&str; 10] =&["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"];
        let mut used_colors = HashMap::<u64, String>::new();
        let mut next_color = 0;

        // Computes color for given material parameters, ensuring that the same parameters lead to the same color
        let mut get_material_color = |rho: f64, E: f64| -> String {
            // Check if the material (rho, E) was already assigned a color
            // If not, assign it the next color in the palette (with wrap-around)
            let key = {
                let mut s = DefaultHasher::new();
                rho.to_bits().hash(&mut s);
                E.to_bits().hash(&mut s);
                s.finish()
            };

            match used_colors.entry(key) {
                Entry::Occupied(entry) => entry.into_mut().clone(),
                Entry::Vacant(entry) => {
                    let value = entry.insert(COLOR_PALETTE[next_color % COLOR_PALETTE.len()].to_string()).clone();
                    next_color += 1;
                    return value;
                },
            }
        };

        // Create new materials from layer information, one material per layer
        // Give them the same name as the layer and assign a new color
        let materials = model.layers.iter().map(|layer| {
            Material {
                name: layer.name.clone(),
                color: get_material_color(layer.rho, layer.E),
                rho: layer.rho,
                E: layer.E,
            }
        }).collect();

        // Create new layers and assign the correct material index
        let layers = model.layers.iter().enumerate().map(|(index, layer)| {
            Layer {
                name: layer.name.clone(),
                material: index,
                height: layer.height.clone(),
            }
        }).collect();

        // Sort the old profile representation (lengths and curvatures) by length
        let mut profile  = model.profile.0;
        profile.sort_by(|a, b| a[0].partial_cmp(&b[0]).expect("Failed to compare floating point values"));

        // Convert old profile definition to line, arc and spiral segments
        let profile = profile.iter().tuple_windows().map(|(prev, next)| {
            // Old segment data
            let length = next[0] - prev[0];
            let kappa0 = prev[1];
            let kappa1 = next[1];

            // Converts a curvature to a radius, with the additional convention that a radius of zero encodes zero curvature
            let curvature_to_radius = |kappa: f64| {
                if kappa != 0.0 { 1.0/kappa } else { 0.0 }
            };

            // Create a segment based on the properties the two curvatures
            // - Both curvatures are zero => create a line segment
            // - Both curvatures are nonzero and equal => create an arc segment
            // - Both curvatures are different => create a spiral segment
            if kappa0 == 0.0 && kappa1 == 0.0 {
                ProfileSegment::Line {
                    parameters: Line {
                        length
                    },
                }
            }
            else if kappa0 == kappa1 {
                ProfileSegment::Arc {
                    parameters: Arc {
                        length,
                        radius: curvature_to_radius(kappa0)
                    }
                }
            }
            else {
                ProfileSegment::Spiral {
                    parameters: Spiral {
                        length,
                        r_start: curvature_to_radius(kappa0),
                        r_end: curvature_to_radius(kappa1),
                    }
                }
            }
        }).collect();

        Self {
            comment: model.comment,
            settings: model.settings,
            dimensions: model.dimensions,
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