pub mod clothoid;
pub mod spline;

#[cfg(test)]
mod tests {
    use iter_num_tools::lin_space;
    use nalgebra::{DVector, vector};
    use crate::fem::elements::beam::PlanarCurve;
    use crate::bow::profile::profile::CurvePoint;
    use crate::bow::profile::segments::clothoid::{ArcInput, ClothoidSegment, LineInput, SpiralInput};
    use crate::bow::profile::segments::spline::{SplineInput, SplineSegment};
    use crate::numerics::differentiation::differentiate_1_to_n;

    #[test]
    fn test_line_segment() {
        let start = CurvePoint::new(1.5, 0.2, vector![2.5, 5.4]);
        let input = LineInput{ length: 0.9 };

        let segment = ClothoidSegment::line(&start, &input);
        test_segment(&start, &segment, 1e-12);
    }

    #[test]
    fn test_arc_segment() {
        let start = CurvePoint::new(1.5, 0.2, vector![2.5, 5.4]);
        let input = ArcInput{ length: 0.9, radius: 5.0 };

        let segment = ClothoidSegment::arc(&start, &input);
        test_segment(&start, &segment, 1e-12);
    }

    #[test]
    fn test_spiral_segment() {
        let start = CurvePoint::new(1.5, 0.2, vector![2.5, 5.4]);
        let input = SpiralInput{ length: 0.9, radius0: 2.0, radius1: -2.0 };

        let segment = ClothoidSegment::spiral(&start, &input);
        test_segment(&start, &segment, 1e-12);
    }

    #[test]
    fn test_spline_segment() {
        let start = CurvePoint::new(1.5, 0.2, vector![2.5, 5.4]);
        let input = SplineInput{ points: vec![[0.0, 0.0], [1.0, 1.0], [2.0, 4.0], [3.0, 9.0]] };

        let segment = SplineSegment::new(&start, &input);
        test_segment(&start, &segment, 1e-3);    // TODO: Derivatives of the spline segment (and its tests) are pretty inaccurate
    }

    fn test_segment<S: PlanarCurve>(start: &CurvePoint, segment: &S, tol: f64) {
        // Check if the segment has the correct startpoint properties
        assert_abs_diff_eq!(segment.s_start(), start.s, epsilon=tol);
        assert_abs_diff_eq!(segment.angle(start.s), start.φ, epsilon=tol);
        assert_abs_diff_eq!(segment.position(start.s), start.r, epsilon=tol);

        // Helper functions for derivatives
        let mut r = |s| {
            DVector::from_column_slice((segment.position(s)).as_slice())  // TODO: Ugly
        };
        let mut drds = |s| {
            DVector::from_column_slice((segment.deriv1(s)).as_slice())  // TODO: Ugly
        };
        let drds2 = |s| {
            DVector::from_column_slice((segment.deriv2(s)).as_slice())  // TODO: Ugly
        };

        // Check the derivatives, angles and curvatures at different points
        for s in lin_space(segment.s_start()..=segment.s_end(), 100) {
            let (drds_num, _) = differentiate_1_to_n(&mut r, s, 0.01);
            let (drds2_num, _) = differentiate_1_to_n(&mut drds, s, 0.01);

            let drds_ana = drds(s);
            let drds2_ana = drds2(s);

            // Check derivative
            assert_abs_diff_eq!(drds_ana, drds_num, epsilon=tol);
            assert_abs_diff_eq!(drds2_ana, drds2_num, epsilon=tol);

            // Check angle
            let angle_ref = f64::atan2(drds_ana[1], drds_ana[0]);
            assert_abs_diff_eq!(segment.angle(s), angle_ref, epsilon=tol);

            // Check curvature
            let curvature_ref = (drds_ana[0]*drds2_ana[1] - drds2_ana[0]*drds_ana[1])/drds_ana.norm().powi(3);
            assert_abs_diff_eq!(segment.curvature(s), curvature_ref, epsilon=tol);
        }
    }
}