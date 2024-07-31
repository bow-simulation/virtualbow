use std::fmt::Debug;
use serde::{Deserialize, Serialize};
use crate::fem::elements::beam::PlanarCurve;
use crate::bow::errors::ModelError;
use crate::bow::profile::profile::CurvePoint;
use crate::bow::profile::segments::clothoid::{ArcInput, ClothoidSegment, LineInput, SpiralInput};
use crate::bow::profile::segments::spline::{SplineInput, SplineSegment};

// Common data type for all possible segment inputs,
// including methods for validation and creating the corresponding segment curves

#[derive(Serialize, Deserialize, PartialEq, Debug)]
#[serde(tag = "type")]
pub enum SegmentInput {
    #[serde(rename = "line")]
    Line(LineInput),

    #[serde(rename = "arc")]
    Arc(ArcInput),

    #[serde(rename = "spiral")]
    Spiral(SpiralInput),

    #[serde(rename = "spline")]
    Spline(SplineInput),
}

impl SegmentInput {
    pub fn create(&self, start: &CurvePoint) -> Box<dyn PlanarCurve> {
        match self {
            SegmentInput::Line(input)   => Box::new(ClothoidSegment::line(start, input)),
            SegmentInput::Arc(input)    => Box::new(ClothoidSegment::arc(start, input)),
            SegmentInput::Spiral(input) => Box::new(ClothoidSegment::spiral(start, input)),
            SegmentInput::Spline(input) => Box::new(SplineSegment::new(start, input)),
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        match self {
            SegmentInput::Line(input)   => input.validate(index),
            SegmentInput::Arc(input)    => input.validate(index),
            SegmentInput::Spiral(input) => input.validate(index),
            SegmentInput::Spline(input) => input.validate(index),
        }
    }
}