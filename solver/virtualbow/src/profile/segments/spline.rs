use iter_num_tools::lin_space;
use nalgebra::{SVector, vector};
use crate::input::Spline;
use crate::profile::profile::CurvePoint;
use virtualbow_fem::elements::beam::geometry::PlanarCurve;
use virtualbow_num::spline::BoundaryCondition::{FirstDerivative, SecondDerivative};
use virtualbow_num::spline::{CubicSpline, Extrapolation};

// Curve segment that is defined by a number of 2d control points interpolated by cubic splines.

pub struct SplineSegment {
    spline_t: CubicSpline,    // t(s): arc length -> parameter
    spline_x: CubicSpline,    // x(t): parameter -> x position
    spline_y: CubicSpline,    // y(t): parameter -> y position
}

impl SplineSegment {
    pub fn new(start: &CurvePoint, input: &Spline) -> SplineSegment {
        let mut x = Vec::<f64>::with_capacity(input.points.len() + 1);
        let mut y = Vec::<f64>::with_capacity(input.points.len() + 1);

        // Add point (0, 0) if missing
        if !input.points.is_empty() && input.points[0] != [0.0, 0.0] {
            x.push(start.position[0]);
            y.push(start.position[1]);
        }

        // Add points from model, relative to starting point
        for point in &input.points {
            x.push(start.position[0] + point[0]);
            y.push(start.position[1] + point[1]);
        }

        assert!(x.len() >= 2, "At least two points are required");

        let N = f64::hypot(x[1] - x[0], y[1] - y[0]);                 // Length of normal vector at start point (magic number motivated by cubic Bezier curve)
        let t = lin_space(0.0..=1.0, x.len()).collect::<Vec<f64>>();    // Linearly spaced curve parameter
        let spline_x = CubicSpline::from_components(&t, &x, false, FirstDerivative(N*f64::cos(start.position[2])), SecondDerivative(0.0));
        let spline_y = CubicSpline::from_components(&t, &y, false, FirstDerivative(N*f64::sin(start.position[2])), SecondDerivative(0.0));

        // Approximate arc length s over curve parameter t

        let k = 50*(t.len() - 1);    // Magic number, integration points per cubic interval
        let t = lin_space(0.0..=1.0, k).collect::<Vec<f64>>();

        let mut s = vec![start.length; k];
        let mut dtds = vec![0.0; k];

        let dsdt = |t| {
            f64::hypot(spline_x.deriv1(t, Extrapolation::Cubic), spline_y.deriv1(t, Extrapolation::Cubic))
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
    fn start(&self) -> f64 {
        self.spline_t.arg_min()
    }

    fn end(&self) -> f64 {
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