use std::{f64::consts::FRAC_PI_2, f64::consts::PI};

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

pub struct Arc {
    l: f64,
    r: f64
}

impl Arc {
    pub fn new(l: f64, r: f64) -> Self {
        Self {
            l,
            r
        }
    }
}

impl PlanarCurve for Arc {
    fn length_start(&self) -> f64 {
        0.0
    }

    fn length_end(&self) -> f64 {
        self.l
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        let φ = self.angle(s);
        vector![
            self.r*f64::sin(φ),
            self.r*(1.0 - f64::cos(φ))
        ]
    }

    fn angle(&self, s: f64) -> f64 {
        s/self.r
    }

    fn curvature(&self, _s: f64) -> f64 {
        1.0/self.r
    }
}

pub struct Wave {
    l: f64,
    k: i32,
    r: f64
}

impl Wave {
    pub fn new(l: f64, k: i32) -> Self {
        Self {
            l,
            k,
            r: l/(PI*(k as f64))
        }
    }

    // Returns center of current arc, angle wrt. center and index
    fn center(&self, s: f64) -> (f64, f64, f64) {
        let n = s/self.l;

        let i = f64::floor(n*(self.k as f64));            // Index of the current arc
        let c = self.r*(1.0 + 2.0*i);                     // Center of the current arc
        let α = (s/self.l*(self.k as f64) - i)*PI;    // Angle from arc center

        (i, c, α)
    }
}

impl PlanarCurve for Wave {
    fn length_start(&self) -> f64 {
        0.0
    }

    fn length_end(&self) -> f64 {
        self.l
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        let (i, c, α) = self.center(s);
        vector![
            c - self.r*f64::cos(α),
            ((-1_i64).pow(i as u32) as f64)*self.r*f64::sin(α)
        ]
    }

    fn angle(&self, s: f64) -> f64 {
        let (i, _, α) = self.center(s);
        ((-1_i64).pow(i as u32) as f64)*(FRAC_PI_2 - α)
    }

    fn curvature(&self, _s: f64) -> f64 {
        unimplemented!()
    }
}

// Makes things a little easier
impl PlanarCurve for Box<dyn PlanarCurve> {
    fn length_start(&self) -> f64 {
        self.as_ref().length_start()
    }

    fn length_end(&self) -> f64 {
        self.as_ref().length_end()
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        self.as_ref().position(s)
    }

    fn angle(&self, s: f64) -> f64 {
        self.as_ref().angle(s)
    }

    fn curvature(&self, s: f64) -> f64 {
        self.as_ref().curvature(s)
    }
}