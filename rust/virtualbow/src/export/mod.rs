pub mod step;
pub mod stl;
/*
use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::{vector, SVector};
use virtualbow_num::fem::elements::beam::geometry::{CrossSection, PlanarCurve};
use crate::geometry::LimbGeometry;

type Point = SVector<f64, 3>;

struct LayerExportInfo {
    name: String,
    color: String,
    sections: Vec<[Point; 4]>
}

struct LimbExportInfo {
    layers: Vec<LayerExportInfo>
}

impl LimbExportInfo {
    fn new(geometry: &LimbGeometry) -> Self {
        // Initialize layers, but without section geometries yet
        let mut layers = geometry.section.layers().iter().map(|layer| LayerExportInfo {
            name: layer.name.clone(),
            color: layer.material.color.clone(),
            sections: vec![],
        }).collect_vec();

        // Lengths and widths at which to evaluate the limb
        let lengths = lin_space(geometry.profile.length_start()..=geometry.profile.length_end(), 10).collect_vec();    // Lengths along the profile curve
        let ratios = lin_space(0.0..=1.0, 10).collect_vec();

        // Profile and cross-section properties
        let points = lengths.iter().map(|&s| geometry.profile.point(s)).collect_vec();
        let widths = ratios.iter().map(|&n| geometry.section.width(n)).collect_vec();
        let bounds = ratios.iter().map(|&n| geometry.section.layer_bounds(n).0).collect_vec();

        // Compute layer section geometry
        for iSection in 0..points.len() {
            let x = points[iSection][0];
            let y = points[iSection][1];
            let z = widths[iSection]/2.0;
            let φ = points[iSection][2];

            for iLayer in 0..layers.len() {
                let y_btm = bounds[iSection][iLayer];
                let y_top = bounds[iSection][iLayer + 1];

                layers[iLayer].sections.push([
                    vector![x - y_top*f64::sin(φ), y + y_top*f64::cos(φ),  z],
                    vector![x - y_top*f64::sin(φ), y + y_top*f64::cos(φ), -z],
                    vector![x - y_btm*f64::sin(φ), y + y_btm*f64::cos(φ),  z],
                    vector![x - y_btm*f64::sin(φ), y + y_btm*f64::cos(φ), -z],
                ]);
            }
        }

        Self {
            layers
        }
    }
}
*/