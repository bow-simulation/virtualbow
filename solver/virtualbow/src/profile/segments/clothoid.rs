use std::f64::consts::{FRAC_PI_2, TAU};
use fresnel::fresnl;
use nalgebra::{SVector, vector};
use crate::input::{Arc, Line, Spiral};
use virtualbow_fem::elements::beam::geometry::PlanarCurve;

// Curve segment whose curvature varies linearly varying over its arc length.
// Can represent a clothoid, circular arc or a straight line depending on the choice of parameters.

pub struct ClothoidCurve {
    x0: f64,
    y0: f64,
    a: f64,
    b: f64,
    c: f64,
    l: f64,
}

impl ClothoidCurve {
    pub fn line(start: [f64; 3], input: &Line) -> ClothoidCurve {
        Self::new(start, input.length, 0.0, 0.0)
    }

    pub fn arc(start: [f64; 3], input: &Arc) -> ClothoidCurve {
        Self::new(start, input.length, Self::radius_to_curvature(input.radius), Self::radius_to_curvature(input.radius))
    }

    pub fn spiral(start: [f64; 3], input: &Spiral) -> ClothoidCurve {
        Self::new(start, input.length, Self::radius_to_curvature(input.radius_start), Self::radius_to_curvature(input.radius_end))
    }

    fn new(start: [f64; 3], l: f64, k0: f64, k1: f64) -> ClothoidCurve {
        ClothoidCurve {
            x0: start[0],
            y0: start[1],
            a: 0.5*(k1 - k0)/l,
            b: k0,
            c: start[2],
            l,
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

impl PlanarCurve for ClothoidCurve {
    fn length(&self) -> f64 {
        self.l
    }

    fn point(&self, s: f64) -> SVector<f64, 2> {
        // The curve's coordinates are (https://en.wikipedia.org/wiki/Tangential_angle)
        // x(s) = x0 + integrate cos(phi(t)) dt from 0 to s
        // y(s) = y0 + integrate sin(phi(t)) dt from 0 to s
        // The solution to this depends on the values of the coefficients
        if self.a > 0.0 {
            // Euler spiral with increasing curvature
            // x(s) = x0 + integrate cos(a*t^2 + b*t + c) dt from 0 to s
            // y(s) = y0 + integrate sin(a*t^2 + b*t + c) dt from 0 to s
            let (Ss, Cs) = fresnl((self.b + 2.0*self.a*s)/f64::sqrt(TAU*self.a));
            let (Sc, Cc) = fresnl(self.b/f64::sqrt(TAU*self.a));
            vector![
                self.x0 + f64::sqrt(FRAC_PI_2/self.a)*(f64::cos(self.b.powi(2)/(4.0*self.a) - self.c)*(Cs - Cc) + f64::sin(self.b.powi(2)/(4.0*self.a) - self.c)*(Ss - Sc)),
                self.y0 + f64::sqrt(FRAC_PI_2/self.a)*(f64::sin(self.b.powi(2)/(4.0*self.a) - self.c)*(Cc - Cs) + f64::cos(self.b.powi(2)/(4.0*self.a) - self.c)*(Ss - Sc))
            ]
        }
        else if self.a < 0.0 {
            // Euler spiral with decreasing curvature
            // Solution from above, but with k0 = -k0, k1 = -k1, y = -y
            let (Ss, Cs) = fresnl((self.b + 2.0*self.a*s)/f64::sqrt(-TAU*self.a));
            let (Sc, Cc) = fresnl(self.b/f64::sqrt(-TAU*self.a));
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
                self.x0 + 1.0/self.b*(f64::sin(self.b*s + self.c) - f64::sin(self.c)),
                self.y0 + 1.0/self.b*(f64::cos(self.c) - f64::cos(self.b*s + self.c))
            ]
        }
        else {
            // Line (zero curvature)
            // x(s) = x0 + integrate cos(c) dt from s0 to s
            // y(s) = y0 + integrate sin(c) dt from s0 to s
            vector![
                self.x0 + f64::cos(self.c)*s,
                self.y0 + f64::sin(self.c)*s
            ]
        }
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

    fn curvature(&self, s: f64) -> f64 {
        2.0*self.a*s + self.b
    }
}