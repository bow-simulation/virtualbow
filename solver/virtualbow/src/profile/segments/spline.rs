use iter_num_tools::lin_space;
use nalgebra::{Isometry2, Point2, SVector, vector};
use crate::input::Spline;
use virtualbow_fem::elements::beam::geometry::PlanarCurve;
use virtualbow_num::spline::BoundaryCondition::{FirstDerivative, SecondDerivative};
use virtualbow_num::spline::{CubicSpline, Extrapolation};

// Curve segment that is defined by a number of 2d control points interpolated by cubic splines.
// The splines are constructed in a local frame (u, v) that is aligned with the start tangent:
// u points along the start direction, v is normal to it. This way the tangent direction at the
// start can be enforced exactly (v'(0) = 0) while the tangent magnitude is left free,
// which minimizes the "overall curvature" of the spline.

pub struct SplineCurve {
    spline_t: CubicSpline,    // t(s): arc length -> parameter
    spline_u: CubicSpline,    // u(t): parameter -> position along start tangent
    spline_v: CubicSpline,    // v(t): parameter -> position normal to start tangent
    frame: Isometry2<f64>,    // Local (u, v) frame at the start point, maps local to global coordinates
}

impl SplineCurve {
    // Minimum tangent magnitude at the start, used as a fallback if the energy-optimal tangent would
    // point backwards or be too short. Dimensionless, since chord-length parametrization makes |r'| ~ 1.
    const MIN_START_SPEED: f64 = 0.1;

    pub fn new(start: [f64; 3], input: &Spline) -> SplineCurve {
        let frame = Isometry2::new(vector![start[0], start[1]], start[2]);

        let mut u = Vec::<f64>::with_capacity(input.points.len() + 1);
        let mut v = Vec::<f64>::with_capacity(input.points.len() + 1);

        // Add point (0, 0) if missing
        if !input.points.is_empty() && input.points[0] != [0.0, 0.0] {
            u.push(0.0);
            v.push(0.0);
        }

        // Add points from input, relative to the starting point, rotated into the local frame
        for point in &input.points {
            let local = frame.inverse_transform_vector(&vector![point[0], point[1]]);
            u.push(local[0]);
            v.push(local[1]);
        }

        assert!(u.len() >= 2, "At least two points are required");

        // Chord-length parametrization
        let mut t = Vec::<f64>::with_capacity(u.len());
        t.push(0.0);
        for i in 1..u.len() {
            let dt = f64::hypot(u[i] - u[i-1], v[i] - v[i-1]);
            assert!(dt > 0.0, "Consecutive points must not coincide");
            t.push(t[i-1] + dt);
        }
        let t_max = t[t.len() - 1];

        // v: Clamped with zero slope at the start, enforces the tangent direction
        // u: Natural at the start, tangent magnitude is determined by minimizing the bending energy
        let spline_v = CubicSpline::from_components(&t, &v, false, FirstDerivative(0.0), SecondDerivative(0.0));
        let mut spline_u = CubicSpline::from_components(&t, &u, false, SecondDerivative(0.0), SecondDerivative(0.0));

        // Fallback if the optimal tangent points backwards or is too short: clamp to minimum magnitude
        if spline_u.deriv1(0.0, Extrapolation::Cubic) < Self::MIN_START_SPEED {
            spline_u = CubicSpline::from_components(&t, &u, false, FirstDerivative(Self::MIN_START_SPEED), SecondDerivative(0.0));
        }

        // Approximate arc length s over curve parameter t
        // (ds/dt is invariant under rotation, so it can be computed in the local frame)

        let k = 50*(t.len() - 1);    // Magic number, integration points per cubic interval
        let t = lin_space(0.0..=t_max, k).collect::<Vec<f64>>();

        let mut s = vec![0.0; k];

        let dsdt = |t| {
            f64::hypot(spline_u.deriv1(t, Extrapolation::Cubic), spline_v.deriv1(t, Extrapolation::Cubic))
        };

        for i in 1..k {
            s[i] = s[i-1] + (t[i] - t[i-1])/6.0*(dsdt(t[i-1]) + 4.0*dsdt((t[i] + t[i-1])/2.0) + dsdt(t[i]));    // Simpson method
        }

        // Spline function for interpolating the arc length, derivatives at the bounds are known
        let spline_t = CubicSpline::from_components(&s, &t, true, FirstDerivative(1.0/dsdt(0.0)), FirstDerivative(1.0/dsdt(t_max)));

        Self {
            spline_t,
            spline_u,
            spline_v,
            frame,
        }
    }
}

impl PlanarCurve for SplineCurve {
    fn length(&self) -> f64 {
        self.spline_t.arg_max()
    }

    fn point(&self, s: f64) -> SVector<f64, 2> {
        let t = self.spline_t.value(s, Extrapolation::Cubic);
        let u = self.spline_u.value(t, Extrapolation::Cubic);
        let v = self.spline_v.value(t, Extrapolation::Cubic);
        (self.frame * Point2::new(u, v)).coords
    }

    fn angle(&self, s: f64) -> f64 {
        let t = self.spline_t.value(s, Extrapolation::Cubic);
        let dudt = self.spline_u.deriv1(t, Extrapolation::Cubic);
        let dvdt = self.spline_v.deriv1(t, Extrapolation::Cubic);
        let tangent = self.frame * vector![dudt, dvdt];
        f64::atan2(tangent[1], tangent[0])
    }

    fn curvature(&self, s: f64) -> f64 {
        // Curvature is invariant under rotation, so it can be computed in the local frame
        let t = self.spline_t.value(s, Extrapolation::Cubic);

        let dudt = self.spline_u.deriv1(t, Extrapolation::Cubic);
        let dvdt = self.spline_v.deriv1(t, Extrapolation::Cubic);

        let dudt2 = self.spline_u.deriv2(t, Extrapolation::Cubic);
        let dvdt2 = self.spline_v.deriv2(t, Extrapolation::Cubic);

        (dudt*dvdt2 - dudt2*dvdt)/f64::hypot(dudt, dvdt).powi(3)
    }
}
