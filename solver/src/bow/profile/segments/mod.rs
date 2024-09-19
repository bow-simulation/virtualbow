pub mod clothoid;
pub mod spline;

#[cfg(test)]
mod tests {
    use nalgebra::vector;
    use crate::bow::profile::profile::CurvePoint;
    use crate::bow::profile::segments::clothoid::{ArcInput, ClothoidSegment, LineInput, SpiralInput};
    use crate::bow::profile::segments::spline::{SplineInput, SplineSegment};
    use crate::fem::elements::beam::geometry::PlanarCurve;

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
        test_segment(&start, &segment, 1e-12);
    }

    fn test_segment<S: PlanarCurve>(start: &CurvePoint, segment: &S, tol: f64) {
        // Check if the segment has the correct startpoint properties
        assert_abs_diff_eq!(segment.s_start(), start.s, epsilon=tol);
        assert_abs_diff_eq!(segment.angle(start.s), start.φ, epsilon=tol);
        assert_abs_diff_eq!(segment.position(start.s), start.r, epsilon=tol);

        // TODO: More tests?
    }
}