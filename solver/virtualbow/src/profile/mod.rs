pub mod profile;
pub mod segments;

#[cfg(test)]
mod tests {
    use approx::assert_abs_diff_eq;
    use iter_num_tools::lin_space;
    use nalgebra::DVector;
    use crate::profile::segments::clothoid::ClothoidCurve;
    use crate::profile::segments::spline::SplineCurve;
    use virtualbow_fem::elements::beam::geometry::PlanarCurve;
    use virtualbow_num::numdiff::differentiate_1_to_n;
    use crate::input::{Arc, Line, Spiral, Spline};

    #[test]
    fn test_line_segment() {
        let start = [2.5, 5.4, 0.2];
        let input = Line{ length: 0.9 };

        let segment = ClothoidCurve::line(start, &input);
        test_curve_properties(start, &segment);
    }

    #[test]
    fn test_arc_segment() {
        let start = [2.5, 5.4, 0.2];
        let input = Arc{ length: 0.9, radius: 5.0 };

        let segment = ClothoidCurve::arc(start, &input);
        test_curve_properties(start, &segment);
    }

    #[test]
    fn test_spiral_segment() {
        let start = [2.5, 5.4, 0.2];
        let input = Spiral{ length: 0.9, radius_start: 2.0, radius_end: -2.0 };

        let segment = ClothoidCurve::spiral(start, &input);
        test_curve_properties(start, &segment);
    }

    #[test]
    fn test_spline_segment() {
        let start = [2.5, 5.4, 0.2];
        let input = Spline{ points: vec![[0.0, 0.0], [1.0, 1.0], [2.0, 4.0], [3.0, 9.0]] };

        let segment = SplineCurve::new(start, &input);
        test_curve_properties(start, &segment);
    }

    // Tests basic properties of a curve, like starting point and relationships between position, angle and curvature
    fn test_curve_properties<S: PlanarCurve>(start: [f64; 3], curve: &S) {
        // Check if the segment has the correct staring point
        assert_abs_diff_eq!(curve.point(0.0)[0], start[0], epsilon=1e-12);
        assert_abs_diff_eq!(curve.point(0.0)[1], start[1], epsilon=1e-12);
        assert_abs_diff_eq!(curve.angle(0.0), start[2], epsilon=1e-12);

        for s in lin_space(0.0..=curve.length(), 100) {
            // Evaluate curve properties for arc length
            let position = curve.position(s);
            let point = curve.point(s);
            let angle = curve.angle(s);
            let curvature = curve.curvature(s);

            // Compute derivative of positions vs arc length
            let mut f_pos = |s: f64| DVector::from_row_slice(&curve.position(s));
            let (derivative, _) = differentiate_1_to_n(&mut f_pos, s, 1e-6);

            // Check 1: position == [point, angle]
            assert_abs_diff_eq!(position[0], point[0], epsilon=1e-12);
            assert_abs_diff_eq!(position[1], point[1], epsilon=1e-12);
            assert_abs_diff_eq!(position[2], angle, epsilon=1e-12);

            // Check 2: tan(angle) = y_pos'/x_pos'
            assert_abs_diff_eq!(f64::tan(angle), derivative[1]/derivative[0], epsilon=1e-6);

            // Check 3: curvature = angle'
            assert_abs_diff_eq!(curvature, derivative[2], epsilon=1e-2);    // The accuracy of this is probably bad due to the spline curve
        }
    }
}
