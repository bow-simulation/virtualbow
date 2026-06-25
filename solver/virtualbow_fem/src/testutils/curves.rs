use std::{f64::consts::FRAC_PI_2, f64::consts::PI};

use nalgebra::{vector, SVector};
use crate::elements::beam::geometry::PlanarCurve;

// Implementation of a straight line curve for use in tests
pub struct LineCurve {
    x: f64,
    y: f64,
    φ: f64,
    l: f64,
}

impl LineCurve {
    pub fn new(start: [f64; 3], l: f64) -> Self {
        Self {
            x: start[0],
            y: start[1],
            φ: start[2],
            l,
        }
    }
}

impl PlanarCurve for LineCurve {
    fn length(&self) -> f64 {
        self.l
    }

    fn point(&self, s: f64) -> SVector<f64, 2> {
        vector![
            self.x + s*f64::cos(self.φ),
            self.y + s*f64::sin(self.φ),
        ]
    }

    fn angle(&self, _s: f64) -> f64 {
        self.φ
    }

    fn curvature(&self, _s: f64) -> f64 {
        0.0
    }
}

// Implementation of a circular arc curve for use in tests
pub struct ArcCurve {
    x: f64,
    y: f64,
    φ: f64,
    l: f64,
    r: f64,
}

impl ArcCurve {
    pub fn new(start: [f64; 3], l: f64, r: f64) -> Self {
        Self {
            x: start[0],
            y: start[1],
            φ: start[2],
            l,
            r
        }
    }
}

impl PlanarCurve for ArcCurve {
    fn length(&self) -> f64 {
        self.l
    }

    fn point(&self, s: f64) -> SVector<f64, 2> {
        vector![
            self.x + self.r*(f64::sin(s/self.r + self.φ) - f64::sin(self.φ)),
            self.y + self.r*(f64::cos(self.φ) - f64::cos(s/self.r + self.φ))
        ]
    }

    fn angle(&self, s: f64) -> f64 {
        self.φ + s/self.r
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

        let i = f64::floor(n*(self.k as f64));        // Index of the current arc
        let c = self.r*(1.0 + 2.0*i);                 // Center of the current arc
        let α = (s/self.l*(self.k as f64) - i)*PI;    // Angle from arc center

        (i, c, α)
    }
}

impl PlanarCurve for Wave {
    fn length(&self) -> f64 {
        self.l
    }

    fn point(&self, s: f64) -> SVector<f64, 2> {
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