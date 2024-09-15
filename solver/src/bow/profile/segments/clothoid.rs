use std::f64::consts::{FRAC_PI_2, TAU};
use fresnel::fresnl;
use nalgebra::{SVector, vector};
use serde::{Deserialize, Serialize};
use crate::fem::elements::beam::PlanarCurve;
use crate::bow::errors::ModelError;
use crate::bow::profile::profile::CurvePoint;

// Curve segment whose curvature varies linearly varying over its arc length.
// Can represent a clothoid, circular arc or a straight line depending on the choice of parameters.

// Line parameters

#[derive(Serialize, Deserialize, Clone, PartialEq, Debug)]
pub struct LineInput {
    pub length: f64
}

impl LineInput {
    pub fn new(length: f64) -> Self {
        Self {
            length
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        if !self.length.is_finite() || self.length <= 0.0 {
            return Err(ModelError::LineSegmentInvalidLength(index, self.length));
        }

        Ok(())
    }
}

// Arc parameters

#[derive(Serialize, Deserialize, Clone, PartialEq, Debug)]
pub struct ArcInput {
    pub length: f64,
    pub radius: f64
}

impl ArcInput {
    pub fn new(length: f64, radius: f64) -> Self {
        Self {
            length,
            radius,
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        if !self.length.is_finite() || self.length <= 0.0 {
            return Err(ModelError::ArcSegmentInvalidLength(index, self.length));
        }
        if !self.radius.is_finite() {
            return Err(ModelError::ArcSegmentInvalidRadius(index, self.radius));
        }

        Ok(())
    }
}

// Spiral parameters

#[derive(Serialize, Deserialize, Clone, PartialEq, Debug)]
pub struct SpiralInput {
    pub length: f64,
    pub radius0: f64,
    pub radius1: f64
}

impl SpiralInput {
    pub fn new(length: f64, radius0: f64, radius1: f64) -> Self {
        Self {
            length,
            radius0,
            radius1
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        if !self.length.is_finite() || self.length <= 0.0 {
            return Err(ModelError::SpiralSegmentInvalidLength(index, self.length));
        }
        if !self.radius0.is_finite() {
            return Err(ModelError::SpiralSegmentInvalidRadius1(index, self.radius0));
        }
        if !self.radius1.is_finite() {
            return Err(ModelError::SpiralSegmentInvalidRadius2(index, self.radius1));
        }

        Ok(())
    }
}

// Common segment implementation for line, arc and spiral input

pub struct ClothoidSegment {
    s0: f64,
    s1: f64,
    x0: f64,
    y0: f64,
    a: f64,
    b: f64,
    c: f64
}

impl ClothoidSegment {
    pub fn line(start: &CurvePoint, input: &LineInput) -> ClothoidSegment {
        Self::new(start, input.length, 0.0, 0.0)
    }

    pub fn arc(start: &CurvePoint, input: &ArcInput) -> ClothoidSegment {
        Self::new(start, input.length, Self::radius_to_curvature(input.radius), Self::radius_to_curvature(input.radius))
    }

    pub fn spiral(start: &CurvePoint, input: &SpiralInput) -> ClothoidSegment {
        Self::new(start, input.length, Self::radius_to_curvature(input.radius0), Self::radius_to_curvature(input.radius1))
    }

    fn new(start: &CurvePoint, l: f64, k0: f64, k1: f64) -> ClothoidSegment {
        let s0 = start.s;
        let s1 = start.s + l;
        let a = 0.5*(k1 - k0)/(s1 - s0);
        let b = (s1*k0 - s0*k1)/(s1 - s0);
        let c = start.φ - b*s0 - a*s0*s0;

        ClothoidSegment {
            s0, s1,
            x0: start.r[0],
            y0: start.r[1],
            a, b, c
        }
    }

    fn radius_to_curvature(r: f64) -> f64 {
        if r != 0.0 {
            1.0/r
        }
        else {
            0.0
        }
    }
}

impl PlanarCurve for ClothoidSegment {
    fn s_start(&self) -> f64 {
        self.s0
    }

    fn s_end(&self) -> f64 {
        self.s1
    }

    fn curvature(&self, s: f64) -> f64 {
        2.0*self.a*s + self.b
    }

    fn angle(&self, s: f64) -> f64 {
        (self.a*s + self.b)*s + self.c
    }

    // Curvature varies linearly between s0 and s1:
    // k(s) = (k1 - k0)/(s1 - s0)*s + (s1*k0 - s0*k1)/(s1 - s0)
    //
    // Therefore, the tangent angle is (https://en.wikipedia.org/wiki/Tangential_angle)
    // phi(s) = phi0 + integrate k(t) dt from s0 to s
    //        = phi0 + (s1*k0 - s0*k1)/(s1 - s0)*(s - s0) + 1/2*(k1 - k0)/(s1 - s0)*(s^2 - s0^2)
    //        = a*s^2 + b*s + c
    //
    // Quadratic coefficients of the tangent angle:
    //
    // a = 0.5*(k1 - k0)/(s1 - s0);
    // b = (s1*k0 - s0*k1)/(s1 - s0);
    // c = phi0 - b*s0 - a*s0*s0;

    fn position(&self, s: f64) -> SVector<f64, 2> {
        // The curve's coordinates are (https://en.wikipedia.org/wiki/Tangential_angle)
        // x(s) = x0 + integrate cos(phi(t)) dt from s0 to s
        // y(s) = y0 + integrate sin(phi(t)) dt from s0 to s
        // The solution to this depends on the values of the coefficients
        if self.a > 0.0 {
            // Euler spiral with increasing curvature
            // x(s) = x0 + integrate cos(a*t^2 + b*t + c) dt from s0 to s
            // y(s) = y0 + integrate sin(a*t^2 + b*t + c) dt from s0 to s
            let (Ss, Cs) = fresnl((self.b + 2.0*self.a*s)/f64::sqrt(TAU*self.a));
            let (Sc, Cc) = fresnl((self.b + 2.0*self.a*self.s0)/f64::sqrt(TAU*self.a));
            vector![
                self.x0 + f64::sqrt(FRAC_PI_2/self.a)*(f64::cos(self.b.powi(2)/(4.0*self.a) - self.c)*(Cs - Cc) + f64::sin(self.b.powi(2)/(4.0*self.a) - self.c)*(Ss - Sc)),
                self.y0 + f64::sqrt(FRAC_PI_2/self.a)*(f64::sin(self.b.powi(2)/(4.0*self.a) - self.c)*(Cc - Cs) + f64::cos(self.b.powi(2)/(4.0*self.a) - self.c)*(Ss - Sc))
            ]
        }
        else if self.a < 0.0 {
            // Euler spiral with decreasing curvature
            // Solution from above, but with k0 = -k0, k1 = -k1, y = -y
            let (Ss, Cs) = fresnl((self.b + 2.0*self.a*s)/f64::sqrt(-TAU*self.a));
            let (Sc, Cc) = fresnl((self.b + 2.0*self.a*self.s0)/f64::sqrt(-TAU*self.a));
            vector![
                self.x0 - f64::sqrt(-FRAC_PI_2/self.a)*(f64::cos(self.c - self.b.powi(2)/(4.0*self.a))*(Cs - Cc) + f64::sin(self.c - self.b.powi(2)/(4.0*self.a))*(Ss - Sc)),
                self.y0 + f64::sqrt(-FRAC_PI_2/self.a)*(f64::sin(self.c - self.b.powi(2)/(4.0*self.a))*(Cc - Cs) + f64::cos(self.c - self.b.powi(2)/(4.0*self.a))*(Ss - Sc))
            ]
        }
        else if self.b != 0.0 {
            // Circle (constant curvature)
            // x(s) = x0 + integrate cos(b*t + c) dt from s0 to s
            // y(s) = y0 + integrate sin(b*t + c) dt from s0 to s
            vector![
                self.x0 + 1.0/self.b*(f64::sin(self.b*s + self.c) - f64::sin(self.b*self.s0 + self.c)),
                self.y0 + 1.0/self.b*(f64::cos(self.b*self.s0 + self.c) - f64::cos(self.b*s + self.c))
            ]
        }
        else {
            // Line (zero curvature)
            // x(s) = x0 + integrate cos(c) dt from s0 to s
            // y(s) = y0 + integrate sin(c) dt from s0 to s
            vector![
                self.x0 + f64::cos(self.c)*(s - self.s0),
                self.y0 + f64::sin(self.c)*(s - self.s0)
            ]
        }
    }
}