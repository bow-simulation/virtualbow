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
//
// Since v'(0) = 0, the curvature at the start is k(0) = v''(0)/u'(0)^2, so prescribing the tangent
// direction alone can still leave a sharp bend there. A cubic spline has only one boundary condition
// per end though, which is already spent on the tangent direction. The v spline therefore carries one
// additional knot inside its first interval, whose value is not a control point but is solved for such
// that v''(0) = 0. The curve then leaves the starting point in the prescribed direction and without
// curvature, like the natural end at the other side, while still interpolating all control points.

pub struct SplineCurve {
    spline_t: CubicSpline,    // t(s): arc length -> parameter
    spline_u: CubicSpline,    // u(t): parameter -> position along start tangent
    spline_v: CubicSpline,    // v(t): parameter -> position normal to start tangent
    frame: Isometry2<f64>,    // Local (u, v) frame at the start point, maps local to global coordinates
}

impl SplineCurve {
    // Minimum tangent magnitude at the start, used as a fallback if the energy-optimal tangent would
    // point backwards or be too short. Relative to the total chord length of the control points,
    // since the index parametrization makes |r'| scale with the overall size of the curve.
    const MIN_START_SPEED: f64 = 0.1;

    // Exponent applied to the distances between the control points when they are spaced out over the
    // curve parameter. 0 gives an index parametrization, 1 a chord length one, 0.5 is centripetal.
    const PARAMETRIZATION_EXPONENT: f64 = 0.5;

    // Position of the additional knot of the v spline within the first interval, as a fraction of it.
    // Moving it closer to the start makes the adjustment more local at the cost of conditioning.
    const START_KNOT_POSITION: f64 = 0.5;

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

        // Total chord length of the points, used as the length scale of the curve
        let mut chord = 0.0;
        let mut t = Vec::<f64>::with_capacity(u.len());
        t.push(0.0);
        for i in 1..u.len() {
            let distance = f64::hypot(u[i] - u[i-1], v[i] - v[i-1]);
            assert!(distance > 0.0, "Consecutive points must not coincide");
            chord += distance;
            t.push(t[i-1] + distance.powf(Self::PARAMETRIZATION_EXPONENT));
        }

        // Normalize the parameter to the range [0, 1]
        let t_max = t[t.len() - 1];
        for value in t.iter_mut() {
            *value /= t_max;
        }

        let min_start_speed = Self::MIN_START_SPEED*chord;

        // Knots of the v spline, with the additional one inserted into the first interval.
        // The u spline keeps the knots of the control points, it doesn't need the extra freedom.
        let mut t_v = Vec::<f64>::with_capacity(t.len() + 1);
        t_v.push(t[0]);
        t_v.push(t[0] + Self::START_KNOT_POSITION*(t[1] - t[0]));
        t_v.extend_from_slice(&t[1..]);

        // Builds the v spline for a given value at the additional knot.
        // Clamped with zero slope at the start, which enforces the tangent direction.
        let build_v = |v_extra: f64| {
            let mut v_v = Vec::<f64>::with_capacity(v.len() + 1);
            v_v.push(v[0]);
            v_v.push(v_extra);
            v_v.extend_from_slice(&v[1..]);

            CubicSpline::from_components(&t_v, &v_v, false, FirstDerivative(0.0), SecondDerivative(0.0))
        };

        // The spline is linear in its values, so v''(0) is an affine function of the value at the
        // additional knot. Evaluating it twice therefore determines the value that makes v''(0) vanish.
        let dv2_0 = build_v(0.0).deriv2(0.0, Extrapolation::Cubic);
        let dv2_1 = build_v(1.0).deriv2(0.0, Extrapolation::Cubic);
        let slope = dv2_1 - dv2_0;

        // The curvature at the start can't be influenced if v''(0) doesn't depend on the additional
        // knot. This shouldn't happen, but if it does, just place the knot on the straight line.
        let v_extra = if slope.is_normal() {
            -dv2_0/slope
        }
        else {
            Self::START_KNOT_POSITION*v[1]
        };

        // v: Zero slope and zero curvature at the start, natural at the end
        // u: Natural at the start, tangent magnitude is determined by minimizing the bending energy
        let spline_v = build_v(v_extra);
        let mut spline_u = CubicSpline::from_components(&t, &u, false, SecondDerivative(0.0), SecondDerivative(0.0));

        // Fallback if the optimal tangent points backwards or is too short: clamp to minimum magnitude
        if spline_u.deriv1(0.0, Extrapolation::Cubic) < min_start_speed {
            spline_u = CubicSpline::from_components(&t, &u, false, FirstDerivative(min_start_speed), SecondDerivative(0.0));
        }

        // Approximate arc length s over curve parameter t
        // (ds/dt is invariant under rotation, so it can be computed in the local frame)

        let k = 50*(t.len() - 1);    // Magic number, integration points per cubic interval
        let t = lin_space(0.0..=1.0, k).collect::<Vec<f64>>();

        let mut s = vec![0.0; k];

        let dsdt = |t| {
            f64::hypot(spline_u.deriv1(t, Extrapolation::Cubic), spline_v.deriv1(t, Extrapolation::Cubic))
        };

        for i in 1..k {
            s[i] = s[i-1] + (t[i] - t[i-1])/6.0*(dsdt(t[i-1]) + 4.0*dsdt((t[i] + t[i-1])/2.0) + dsdt(t[i]));    // Simpson method
        }

        // Spline function for interpolating the arc length, derivatives at the bounds are known
        let spline_t = CubicSpline::from_components(&s, &t, true, FirstDerivative(1.0/dsdt(0.0)), FirstDerivative(1.0/dsdt(1.0)));

        Self {
            spline_t,
            spline_u,
            spline_v,
            frame,
        }
    }

    // Positions (x, y, φ) of the control points that lie inside the curve, i.e. excluding
    // the ones at the start and the end, which are already nodes of the profile curve.
    pub fn interior_nodes(&self) -> Vec<[f64; 3]> {
        let t = self.spline_u.args();
        let u = self.spline_u.vals();

        // The v spline has an additional knot, so its values are not aligned with the control
        // points and have to be evaluated. At a knot this returns the stored value exactly.
        (1..t.len() - 1).map(|i| {
            let v = self.spline_v.value(t[i], Extrapolation::Cubic);
            let point = (self.frame * Point2::new(u[i], v)).coords;

            let dudt = self.spline_u.deriv1(t[i], Extrapolation::Cubic);
            let dvdt = self.spline_v.deriv1(t[i], Extrapolation::Cubic);
            let tangent = self.frame * vector![dudt, dvdt];

            [point[0], point[1], f64::atan2(tangent[1], tangent[0])]
        }).collect()
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
