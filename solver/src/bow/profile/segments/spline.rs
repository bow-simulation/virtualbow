// Curve segment described by two cubic splines in x and y directon

use iter_num_tools::lin_space;
use nalgebra::{SVector, vector};
use serde::{Deserialize, Serialize};
use crate::bow::errors::ModelError;
use crate::bow::profile::profile::CurvePoint;
use crate::fem::elements::beam::geometry::PlanarCurve;
use crate::numerics::cubic_spline::BoundaryCondition::{FirstDerivative, SecondDerivative};
use crate::numerics::cubic_spline::{CubicSpline, Extrapolation};

// Curve segment that is defined by a number of 2d control points interpolated by cubic splines.

#[derive(Serialize, Deserialize, Clone, PartialEq, Debug)]
pub struct SplineInput {
    pub points: Vec<[f64; 2]>
}

impl SplineInput {
    pub fn new(points: Vec<[f64; 2]>) -> Self {
        Self {
            points
        }
    }

    pub fn validate(&self, index: usize) -> Result<(), ModelError> {
        if self.points.len() < 2 {
            return Err(ModelError::SplineSegmentTooFewPoints(index, self.points.len()));
        }
        for point in &self.points {
            if !point[0].is_finite() || !point[1].is_finite() {
                return Err(ModelError::SplineSegmentInvalidPoint(index, *point));
            }
        }

        Ok(())
    }
}

pub struct SplineSegment {
    spline_t: CubicSpline,    // t(s): arc length -> parameter
    spline_x: CubicSpline,    // x(t): parameter -> x position
    spline_y: CubicSpline,    // y(t): parameter -> y position
}

impl SplineSegment {
    pub fn new(start: &CurvePoint, input: &SplineInput) -> SplineSegment {
        let mut x = Vec::<f64>::with_capacity(input.points.len() + 1);
        let mut y = Vec::<f64>::with_capacity(input.points.len() + 1);

        // Add point (0, 0) if missing
        if !input.points.is_empty() && input.points[0] != [0.0, 0.0] {
            x.push(start.r[0]);
            y.push(start.r[1]);
        }

        // Add points from model, relative to starting point
        for point in &input.points {
            x.push(start.r[0] + point[0]);
            y.push(start.r[1] + point[1]);
        }

        assert!(x.len() >= 2, "At least two points are required");

        let N = f64::hypot(x[1] - x[0], y[1] - y[0]);                 // Length of normal vector at start point (magic number motivated by cubic Bezier curve)
        let t = lin_space(0.0..=1.0, x.len()).collect::<Vec<f64>>();    // Linearly spaced curve parameter
        let spline_x = CubicSpline::from_components(&t, &x, false, FirstDerivative(N*f64::cos(start.φ)), SecondDerivative(0.0));
        let spline_y = CubicSpline::from_components(&t, &y, false, FirstDerivative(N*f64::sin(start.φ)), SecondDerivative(0.0));

        // Approximate arc length s over curve parameter t

        let k = 50*(t.len() - 1);    // Magic number, integration points per cubic interval
        let t = lin_space(0.0..=1.0, k).collect::<Vec<f64>>();

        let mut s = vec![start.s; k];
        let mut dtds = vec![0.0; k];

        let dsdt = |t| {
            return f64::hypot(spline_x.deriv1(t, Extrapolation::Cubic), spline_y.deriv1(t, Extrapolation::Cubic));
        };

        for i in 1..k {
            s[i] = s[i-1] + (t[i] - t[i-1])/6.0*(dsdt(t[i-1]) + 4.0*dsdt((t[i] + t[i-1])/2.0) + dsdt(t[i]));    // Simpson method
            dtds[i] = 1.0/dsdt(t[i]);
        }

        //let spline_t = CubicSpline::from_components(&s, &t, true, SecondDerivative(0.0), SecondDerivative(0.0));
        let spline_t = CubicSpline::from_components(&s, &t, true, FirstDerivative(1.0/dsdt(0.0)), FirstDerivative(1.0/dsdt(1.0)));

        Self {
            spline_t,
            spline_x,
            spline_y,
        }
    }
}

impl PlanarCurve for SplineSegment {
    fn s_start(&self) -> f64 {
        self.spline_t.arg_min()
    }

    fn s_end(&self) -> f64 {
        self.spline_t.arg_max()
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        let t = self.spline_t.value(s, Extrapolation::Cubic);
        vector![
            self.spline_x.value(t, Extrapolation::Cubic),
            self.spline_y.value(t, Extrapolation::Cubic)
        ]
    }

    fn angle(&self, s: f64) -> f64 {
        let t = self.spline_t.value(s, Extrapolation::Cubic);
        f64::atan2(
            self.spline_y.deriv1(t, Extrapolation::Cubic),
            self.spline_x.deriv1(t, Extrapolation::Cubic)
        )
    }

    fn curvature(&self, s: f64) -> f64 {
        let t = self.spline_t.value(s, Extrapolation::Cubic);

        let dxdt = self.spline_x.deriv1(t, Extrapolation::Cubic);
        let dydt = self.spline_y.deriv1(t, Extrapolation::Cubic);

        let dxdt2 = self.spline_x.deriv2(t, Extrapolation::Cubic);
        let dydt2 = self.spline_y.deriv2(t, Extrapolation::Cubic);

        (dxdt*dydt2 - dxdt2*dydt)/f64::hypot(dxdt, dydt).powi(3)
    }
}