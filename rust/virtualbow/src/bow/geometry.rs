use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::{DMatrix, DVector, SVector, vector};
use crate::bow::errors::ModelError;
use crate::bow::input::BowInput;
use crate::bow::profile::profile::{CurvePoint, ProfileCurve};
use crate::bow::sections::section::LayeredCrossSection;
use crate::fem::elements::beam::geometry::{CrossSection, PlanarCurve};
use crate::fem::elements::beam::linear::LinearBeamSegment;

pub struct LimbGeometry {
    pub profile: ProfileCurve,           // Limb profile curve
    pub section: LayeredCrossSection,    // Limb cross sections
}

impl LimbGeometry {
    pub fn new(input: &BowInput) -> Result<Self, ModelError> {
        // Profile curve with starting point according to dimension settings
        let start = CurvePoint::new(0.0, input.dimensions.handle_angle, vector![0.5*input.dimensions.handle_length, input.dimensions.handle_setback]);
        let profile = ProfileCurve::new(start, &input.profile.segments)?;

        // Section properties according to layers, materials and alignment to the profile curve
        // Layers in the mode definition are from back to belly, but here we define the layers from belly to back (direction of the y axis), so the model layers are reversed
        let layers = input.layers.iter().cloned().rev().collect();
        let section = LayeredCrossSection::new(profile.length(), &input.width, &layers, &input.materials, &input.profile.alignment)?;

        // Check for self-intersecting geometry, which is the case when the thickness of the limb is higher than the radius of curvature
        // Since we can't check this analytically, we check for a fixed number of points along the length of the limb
        for s in lin_space(profile.s_start()..=profile.s_end(), 1000) {  // TODO: Magic number
            let kappa = profile.curvature(s);
            let (bounds, _) = section.layer_bounds(s);

            let y_belly = bounds[0];                // At least one layer, ensured by the section
            let y_back = bounds[bounds.len()-1];    // At least one layer, ensured by the section

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
            section
        })
    }

    // Divides the given curve into a number of equally spaced elements.
    // Returns a list of elements as well as the arc lengths, positions and angles of the nodes.
    pub fn discretize(&self, n_eval_points: usize, n_elements: usize) -> DiscreteLimbGeometry {
        // Arc lengths along the profile where the element nodes are placed and their positions
        let s_nodes = lin_space(self.profile.s_start()..=self.profile.s_end(), n_elements + 1).collect_vec();
        let u_nodes = s_nodes.iter().map(|&s| self.profile.point(s)).collect_vec();
        let y_nodes = s_nodes.iter().map(|&s| self.section.layer_bounds(s).0).collect_vec();

        // Equidistant evaluation points along the length of the limb
        let s_eval = lin_space(self.profile.s_start()..=self.profile.s_end(), n_eval_points).collect_vec();
        let y_eval = s_eval.iter().map(|&s| self.section.layer_bounds(s).0).collect_vec();

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

        let strain_eval = s_eval.iter().map(|&s| self.section.strain_eval(s)).collect();
        let stress_eval = s_eval.iter().map(|&s| self.section.stress_eval(s)).collect();

        let position = s_eval.iter().map(|&s| self.profile.point(s).into()).collect();
        let width = s_eval.iter().map(|&s| self.section.width(s)).collect();
        let height = s_eval.iter().map(|&s| self.section.height(s)).collect();

        DiscreteLimbGeometry {
            segments,
            s_nodes,
            u_nodes,
            y_nodes,
            s_eval,
            y_eval,
            strain_eval,
            stress_eval,
            position,
            width,
            height
        }
    }
}

// TODO: Return values s_nodes, u_node might not be needed if the evaluation works properly
pub struct DiscreteLimbGeometry {
    pub segments: Vec<LinearBeamSegment>,    // Linear beam segment properties
    pub s_nodes: Vec<f64>,                   // Arc lengths of the element nodes
    pub u_nodes: Vec<SVector<f64, 3>>,       // Positions (x, y, φ) of the element nodes
    pub y_nodes: Vec<DVector<f64>>,          // Layer bounds at nodes (y in cross section coordinates)

    pub s_eval: Vec<f64>,                    // Arc lengths at which the limb quantities are evaluated (positions, forces, ...)
    pub y_eval: Vec<DVector<f64>>,           // Layer bounds at eval points (y in cross section coordinates)
    pub strain_eval: Vec<DMatrix<f64>>,      // Strain evaluation matrices for each evaluation point
    pub stress_eval: Vec<DMatrix<f64>>,      // Stress evaluation matrices for each evaluation point

    // TODO: Unify with rest
    pub position: Vec<SVector<f64, 3>>,
    pub width: Vec<f64>,
    pub height: Vec<f64>
}

#[cfg(test)]
mod tests {
    use std::fmt::{Debug, Formatter};
    use crate::bow::input::{Layer, Profile};
    use crate::bow::profile::input::SegmentInput;
    use crate::bow::profile::segments::clothoid::{ArcInput, LineInput};
    use crate::bow::sections::section::LayerAlignment;
    use super::*;

    // To make tests below compile
    impl Debug for LimbGeometry {
        fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
            write!(f, "LimbGeometry")
        }
    }

    #[test]
    fn test_error_conditions() {
        let mut input = BowInput::default();
        input.layers = vec![Layer::new("Unnamed", "Default", vec![(0.0, 0.01), (1.0, 0.01)])];

        // 1. Profile curve with no self-intersection
        input.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Line(LineInput::new(1.0))]);
        let _geometry = LimbGeometry::new(&input).unwrap();

        // 2. Profile that produces a self-intersection at the back
        input.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Arc(ArcInput::new(1.0, 0.001))]);
        assert_matches!(LimbGeometry::new(&input), Err(ModelError::GeometrySelfIntersectionBack(0.0)));

        // 3. Profile that produces a self-intersection at the belly
        input.profile = Profile::new(LayerAlignment::SectionCenter, vec![SegmentInput::Arc(ArcInput::new(1.0, -0.001))]);
        assert_matches!(LimbGeometry::new(&input), Err(ModelError::GeometrySelfIntersectionBelly(0.0)));
    }
}