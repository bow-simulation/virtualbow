use nalgebra::{vector, SVector};
use crate::fem::elements::beam::geometry::PlanarCurve;

// Simple straight line of given length for use in tests

pub struct Line {
    l: f64
}

impl Line {
    pub fn new(l: f64) -> Self {
        Self {
            l
        }
    }
}

impl PlanarCurve for Line {
    fn length_start(&self) -> f64 {
        0.0
    }

    fn length_end(&self) -> f64 {
        self.l
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        vector![s, 0.0]
    }

    fn angle(&self, _s: f64) -> f64 {
        0.0
    }

    fn curvature(&self, _s: f64) -> f64 {
        0.0
    }
}