use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::{DVector, SVector};
use serde::{Deserialize, Serialize};
use crate::errors::ModelError;
use crate::input::{BowModel, DrawLength};
use crate::profile::profile::{CurvePoint, ProfileCurve};
use crate::sections::section::LayeredCrossSection;
use virtualbow_fem::elements::beam::geometry::{CrossSection, PlanarCurve};
use virtualbow_fem::elements::beam::linear::LinearBeamSegment;
use crate::output::LimbInfo;

pub struct LimbGeometry {
    pub profile: ProfileCurve,           // Limb profile curve
    pub section: LayeredCrossSection,    // Limb cross sections
    pub draw: DrawInfo                   // Info about brace and draw positions
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct DiscreteLimbGeometry {
    pub segments: Vec<LinearBeamSegment>,    // Linear beam segment properties
    pub n_nodes: Vec<f64>,                   // Relative lengths of the element nodes
    pub s_nodes: Vec<f64>,                   // Arc lengths of the element nodes
    pub k_nodes: Vec<f64>,                   // Curvatures at the element nodes
    pub p_nodes: Vec<[f64; 3]>,              // Positions (x, y, φ) of the element nodes
    pub y_nodes: Vec<DVector<f64>>,          // Layer bounds (back to belly) at nodes (y in cross-section coordinates)
    pub h_nodes: Vec<DVector<f64>>,          // Layer heights (back to belly) at nodes

    pub p_control: Vec<[f64; 3]>,     // Positions (x, y, φ) of the control points

    pub n_eval: Vec<f64>,                    // Relative lengths at which the limb quantities are evaluated
    pub s_eval: Vec<f64>,                    // Arc lengths at which the limb quantities are evaluated
    pub k_eval: Vec<f64>,                    // Curvatures at the eval points
    pub p_eval: Vec<[f64; 3]>,               // Positions (x, y, φ) of the evaluation points
    pub y_eval: Vec<DVector<f64>>,           // Layer bounds at eval points (y in cross-section coordinates)
    pub h_eval: Vec<DVector<f64>>,           // Layer heights at eval points
    pub w_eval: Vec<f64>,                    // Widths at eval points

    pub strain_eval: Vec<Vec<SVector<f64, 3>>>,      // Strain evaluation matrices for each evaluation point
    pub stress_eval: Vec<Vec<SVector<f64, 3>>>,      // Stress evaluation matrices for each evaluation point

    pub draw: DrawInfo
}

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct DrawInfo {
    pub pivot_point: f64,                // Position of the pivot point
    pub brace_ref: f64,                  // Reference position for measurement of the brace height
    pub draw_ref: f64,                   // Reference position for measurement of the draw length
    pub brace_pos: f64,                  // Position of the braced string
    pub draw_pos: f64,                   // Position of the fully drawn string
    pub power_stroke: f64,               // Difference between brace height and full draw (positive)
}

impl LimbGeometry {
    pub fn new(input: &BowModel) -> Result<Self, ModelError> {
        // Section properties according to layers, materials and alignment to the profile curve
        let section = LayeredCrossSection::new(&input.section)?;

        // Determine rigid parameters of the handle according to selection (flexible/rigid)
        let rigid_handle = &input.handle.to_rigid();

        // Profile curve with starting point according to the rigid handle length and angle
        // The pivot point of the handle determines the brace and draw offsets, while the profile curve always starts at y = 0
        let start = CurvePoint::new(0.0, [0.5*rigid_handle.length, 0.0, rigid_handle.angle]);
        let profile = ProfileCurve::new(start, &input.profile.segments)?;

        // Calculate the eccentricity, i.e. the distance between the reference point (belly) and the profile curve at the root of the limb.
        let eccentricity = section.section_bounds(0.0).1;


        // The pivot point position is the handle parameter (which is measured from the belly side), corrected by the distance between profile and belly
        let pivot_point = eccentricity*f64::cos(rigid_handle.angle) - rigid_handle.pivot;

        // The brace reference point is currently identical to the pivot point
        // The draw reference is either the pivot point (standard definition) or adjusted by 1.75in (AMO definition).
        let brace_ref = pivot_point;
        let draw_ref = match input.draw.draw_length {
            DrawLength::Standard(_) => pivot_point,
            DrawLength::Amo(_) => pivot_point + 1.75*0.0254
        };

        // Position of the string at brace and full draw as determined by the reference points
        let brace_pos = brace_ref - input.draw.brace_height;
        let draw_pos = draw_ref - input.draw.draw_length.value();
        let power_stroke = brace_pos - draw_pos;
        let draw = DrawInfo{
            pivot_point,
            brace_ref,
            draw_ref,
            brace_pos,
            draw_pos,
            power_stroke,
        };

        // Check for self-intersecting geometry, which is the case when the thickness of the limb is higher than the radius of curvature.
        // Since we can't check this analytically, we check for a fixed number of points along the length of the limb.
        for s in lin_space(profile.start()..=profile.end(), 1000) {  // Magic number
            let kappa = profile.curvature(s);
            let (y_back, y_belly) = section.section_bounds(profile.normalize(s));

            // Intersection at the back side happens when the curvature is positive, i.e. curved in the back direction and the y coordinate of the back is larger or equal to the radius of curvature
            // Intersection at the belly side happens when the curvature is negative, i.e. curved in the belly direction and the y coordinate of the belly is larger or equal to the radius of curvature
            if kappa > 0.0 && y_back >= 1.0/kappa {
                return Err(ModelError::GeometrySelfIntersectionBack(s));
            }
            else if kappa < 0.0 && y_belly <= 1.0/kappa {
                return Err(ModelError::GeometrySelfIntersectionBelly(s));
            }
        }

        Ok(Self {
            profile,
            section,
            draw
        })
    }

    // Divides the given curve into a number of equally spaced elements.
    // Returns a list of elements as well as the arc lengths, positions and angles of the nodes.
    pub fn discretize(&self, n_eval_points: usize, n_elements: usize) -> DiscreteLimbGeometry {
        // Arc lengths and normalized positions along the profile where the element nodes are placed
        let s_nodes = lin_space(self.profile.start()..=self.profile.end(), n_elements + 1).collect_vec();
        let n_nodes = s_nodes.iter().map(|&s| self.profile.normalize(s)).collect_vec();
        let k_nodes = s_nodes.iter().map(|&s| self.profile.curvature(s)).collect_vec();
        let p_nodes = s_nodes.iter().map(|&s| self.profile.point(s)).collect_vec();
        let (y_nodes, h_nodes): (Vec<_>, Vec<_>) = n_nodes.iter().map(|&n| self.section.layer_bounds(n)).unzip();

        // Control points of the profile curve
        //let k_control = s_control.iter().map(|&s| self.profile.curvature(s)).collect_vec();                    // TODO: Implement
        let p_control = self.profile.get_nodes().iter().map(|node| node.position).collect();    // TODO: Make those conversions unnecessary by using a single format for curve points

        // Equidistant evaluation points along the length of the limb
        let s_eval = lin_space(self.profile.start()..=self.profile.end(), n_eval_points).collect_vec();
        let n_eval = s_eval.iter().map(|&s| self.profile.normalize(s)).collect_vec();
        let y_eval = n_eval.iter().map(|&n| self.section.layer_bounds(n).0).collect_vec();
        let h_eval = n_eval.iter().map(|&n| self.section.layer_bounds(n).1).collect_vec();    // TODO: Collect in one step

        let segments = s_nodes.iter().tuple_windows().enumerate().map(|(i, (&s0, &s1))| {
            // TODO: Better solution for numerical issues?
            let tolerance = 1e-9;

            // TODO: More efficient implementation than filtering each time
            let s_eval = if i == 0 {
                s_eval.iter().copied().filter(|&s| s >= s0 - tolerance && s <= s1 ).collect_vec()    // Include left boundary with tolerance
            }
            else if i == n_elements - 1 {
                s_eval.iter().copied().filter(|&s| s > s0 && s <= s1 + tolerance ).collect_vec()    // Include right boundary with tolerance
            }
            else {
                s_eval.iter().copied().filter(|&s| s > s0 && s <= s1 ).collect_vec()    // Exclude left boundary
            };

            LinearBeamSegment::new(&self.profile, &self.section, s0, s1, &s_eval)
        }).collect();

        let k_eval = s_eval.iter().map(|&s| self.profile.curvature(s)).collect_vec();
        let p_eval = s_eval.iter().map(|&s| self.profile.point(s)).collect();
        let w_eval = n_eval.iter().map(|&n| self.section.width(n)).collect();

        let strain_eval = n_eval.iter().map(|&n| self.section.strain_recovery(n)).collect();
        let stress_eval = n_eval.iter().map(|&n| self.section.stress_recovery(n)).collect();

        DiscreteLimbGeometry {
            segments,
            n_nodes,
            s_nodes,
            k_nodes,
            p_nodes,
            y_nodes,
            h_nodes,
            p_control,
            n_eval,
            s_eval,
            y_eval,
            strain_eval,
            stress_eval,
            k_eval,
            p_eval,
            w_eval,
            h_eval,
            draw: self.draw.clone(),
        }
    }
}

impl DiscreteLimbGeometry {
    pub fn to_limb_info(&self) -> LimbInfo {
        LimbInfo {
            length: self.s_eval.clone(),
            position_eval: self.p_eval.clone(),
            position_control: self.p_control.clone(),
            curvature_eval: self.k_eval.clone(),
            width: self.w_eval.clone(),
            height: self.h_eval.iter().map(|h| h.sum()).collect(),
            bounds: self.y_eval.iter().map(|y| y.data.clone().into()).collect(),    // TODO: Use map_into()
            ratio: self.n_eval.clone(),
            heights: self.h_eval.iter().map(|h| h.data.clone().into()).collect(),    // TODO: Use map_into()
            pivot_point: self.draw.pivot_point,
        }
    }
}

impl TryInto<Vec<u8>> for LimbInfo {
    type Error = ModelError;

    // Conversion into MsgPack byte array
    fn try_into(self) -> Result<Vec<u8>, Self::Error> {
        rmp_serde::to_vec_named(&self).map_err(ModelError::OutputEncodeMsgPackError)  // TODO: Bett error type?
    }
}

impl TryFrom<&[u8]> for LimbInfo {
    type Error = ModelError;

    // Conversion from MsgPack byte array
    fn try_from(value: &[u8]) -> Result<Self, Self::Error> {
        rmp_serde::from_slice(value).map_err(ModelError::OutputDecodeMsgPackError)
    }
}

#[cfg(test)]
mod tests {
    use assert_matches::assert_matches;
    use std::fmt::{Debug, Formatter};
    use crate::input::{Arc, Height, Layer, Material, Line, Profile, ProfileSegment, Section, LayerAlignment, Width};
    use super::*;

    // To make tests below compile
    impl Debug for LimbGeometry {
        fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
            write!(f, "LimbGeometry")
        }
    }

    #[test]
    fn test_error_conditions() {
        let mut input = BowModel {
            section: Section {
                alignment: LayerAlignment::SectionCenter,
                width: Width::linear(0.04, 0.01),
                materials: vec![Material::new("Unnamed", "#000000", 600.0, 12e9, 6e9)],
                layers: vec![Layer::new("Default", "Unnamed", Height::constant(0.01))]
            },
            ..BowModel::example()
        };

        // 1. Profile curve with no self-intersection
        input.profile = Profile::new(vec![ProfileSegment::Line(Line::new(1.0))]);
        assert_matches!(LimbGeometry::new(&input), Ok(_));

        // 2. Profile that produces a self-intersection at the back
        input.profile = Profile::new(vec![ProfileSegment::Arc(Arc::new(1.0, 0.001))]);
        assert_matches!(LimbGeometry::new(&input), Err(ModelError::GeometrySelfIntersectionBack(0.0)));

        // 3. Profile that produces a self-intersection at the belly
        input.profile = Profile::new(vec![ProfileSegment::Arc(Arc::new(1.0, -0.001))]);
        assert_matches!(LimbGeometry::new(&input), Err(ModelError::GeometrySelfIntersectionBelly(0.0)));
    }
}