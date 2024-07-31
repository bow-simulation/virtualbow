use std::f64::consts::{PI, TAU};

// Normalizes an angle to a range of [-PI, PI] by adding or subtracting full turns of 2*PI.
// Implementation based on https://stackoverflow.com/a/2323034 and discussion
pub fn normalize_angle(mut angle: f64) -> f64 {
    // Reduce the angle to the range [0, 2*PI] by a modulo operation (https://stackoverflow.com/a/57342011)
    angle = angle.rem_euclid(TAU);

    // If the result is larger than PI, subtract 2*PI to get a range of [-PI, PI]
    if angle > PI {
        angle - TAU
    }
    else {
        angle
    }
}

#[cfg(test)]
mod tests {
    use crate::numerics::functions::normalize_angle;
    use std::f64::consts::TAU;

    #[test]
    fn test_normalize_angle() {
        // Input = Output for angles in [-pi, pi]
        assert_abs_diff_eq!(normalize_angle( 0.1),  0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-0.1), -0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle( 3.1),  3.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-3.1), -3.1, epsilon=1e-12);

        // Invariant against adding tau = 2*pi
        assert_abs_diff_eq!(normalize_angle( 0.1 + TAU),  0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-0.1 + TAU), -0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle( 3.1 + TAU),  3.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-3.1 + TAU), -3.1, epsilon=1e-12);

        // Invariant against subtracting tau = 2*pi
        assert_abs_diff_eq!(normalize_angle( 0.1 - TAU),  0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-0.1 - TAU), -0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle( 3.1 - TAU),  3.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-3.1 - TAU), -3.1, epsilon=1e-12);

        // Invariant against adding multiples of tau = 2*pi
        assert_abs_diff_eq!(normalize_angle( 0.1 + 3.0*TAU),  0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-0.1 + 3.0*TAU), -0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle( 3.1 + 3.0*TAU),  3.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-3.1 + 3.0*TAU), -3.1, epsilon=1e-12);

        // Invariant against subtracting multiples of tau = 2*pi
        assert_abs_diff_eq!(normalize_angle( 0.1 - 3.0*TAU),  0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-0.1 - 3.0*TAU), -0.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle( 3.1 - 3.0*TAU),  3.1, epsilon=1e-12);
        assert_abs_diff_eq!(normalize_angle(-3.1 - 3.0*TAU), -3.1, epsilon=1e-12);
    }
}