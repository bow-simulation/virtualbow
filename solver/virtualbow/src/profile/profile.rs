use nalgebra::SVector;
use crate::errors::ModelError;
use crate::input::ProfileSegment;
use crate::profile::segments::clothoid::ClothoidCurve;
use crate::profile::segments::spline::SplineCurve;
use virtualbow_fem::elements::beam::geometry::PlanarCurve;
use virtualbow_num::bisection::bisect_right_by;

#[derive(Copy ,Clone, PartialEq)]
pub struct CurvePoint {
    pub length: f64,                // Arc length
    pub position: [f64; 3],         // Position and tangent angle
}

impl CurvePoint {
    pub fn new(length: f64, position: [f64; 3]) -> CurvePoint {
        CurvePoint {
            length,
            position
        }
    }

    pub fn zero() -> CurvePoint {
        CurvePoint {
            length: 0.0,
            position: [0.0; 3]
        }
    }
}

pub struct ProfileCurve {
    segments: Vec<Box<dyn PlanarCurve>>,    // List of segments that make up the profile curve
    nodes: Vec<CurvePoint>,                 // List of nodes at the start and end of each segment
    interior: Vec<[f64; 3]>,                // Positions (x, y, φ) of the nodes inside the segments, e.g. the control points of a spline
}

impl ProfileCurve {
    // TODO: Pass [f64; 3] by value either everywhere or nowhere (-> by refernce instead)
    pub fn new(start: [f64; 3], segment_inputs: &[ProfileSegment]) -> Result<ProfileCurve, ModelError> {
        if segment_inputs.is_empty() {
            return Err(ModelError::ProfileNoSegments);
        }

        let mut nodes = Vec::with_capacity(segment_inputs.len() + 1);
        let mut segments = Vec::with_capacity(segment_inputs.len());
        let mut interior = Vec::new();

        // The first node is the starting point at arc length = 0
        nodes.push(CurvePoint::new(0.0, start));

        // Create curve segments for each model in the list, using the currently last node as the starting point.
        // Add each segment's endpoint to the nodes as the starting point for the next segment.
        for (index, input) in segment_inputs.iter().enumerate() {
            // Make sure the input is valid
            input.validate(index)?;

            // Construct nodes and segment
            let node_start = nodes.last().unwrap();    // The last node is the starting point of the new segment (unwrap is okay because of previous push)
            let (segment, segment_interior) = Self::create_curve(input, node_start.position);    // Create the new segment from its input and starting point, along with the nodes inside of it
            let node_end = CurvePoint::new(node_start.length + segment.length(), segment.position(segment.length()));    // Compute endpoint of the segment, advance arc length by length of the segment

            // Add both to the accumulated curve
            nodes.push(node_end);
            segments.push(segment);
            interior.extend(segment_interior);
        }

        Ok(Self{
            segments,
            nodes,
            interior,
        })
    }

    pub fn get_nodes(&self) -> &[CurvePoint] {
        &self.nodes
    }

    pub fn get_interior_nodes(&self) -> &[[f64; 3]] {
        &self.interior
    }

    // Creates the curve for the given segment input, together with the positions of the
    // nodes that lie inside of it. Only spline segments have such interior nodes.
    fn create_curve(segment: &ProfileSegment, start: [f64; 3]) -> (Box<dyn PlanarCurve>, Vec<[f64; 3]>) {
        match segment {
            ProfileSegment::Line(input)   => (Box::new(ClothoidCurve::line(start, input)), vec![]),
            ProfileSegment::Arc(input)    => (Box::new(ClothoidCurve::arc(start, input)), vec![]),
            ProfileSegment::Spiral(input) => (Box::new(ClothoidCurve::spiral(start, input)), vec![]),
            ProfileSegment::Spline(input) => {
                let curve = SplineCurve::new(start, input);
                let interior = curve.interior_nodes();
                (Box::new(curve), interior)
            }
        }
    }

    fn find_segment_index(&self, s: f64) -> usize {
        bisect_right_by(&self.nodes, |node| node.length.partial_cmp(&s).expect("Failed to compare floating point values"))
    }
}

impl PlanarCurve for ProfileCurve {
    fn length(&self) -> f64 {
        self.nodes.last().unwrap().length    // Unwrap is ensured by construction
    }

    fn point(&self, s: f64) -> SVector<f64, 2> {
        let index = self.find_segment_index(s);
        self.segments[index].point(s - self.nodes[index].length)
    }

    fn angle(&self, s: f64) -> f64 {
        let index = self.find_segment_index(s);
        self.segments[index].angle(s - self.nodes[index].length)
    }

    fn curvature(&self, s: f64) -> f64 {
        let index = self.find_segment_index(s);
        self.segments[index].curvature(s - self.nodes[index].length)
    }
}