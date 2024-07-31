use std::ops::{Add, Mul};
use super::bisection::bisect_right;
use num::Float;

// Spline that performs linear interpolation between its construction points
// T is a floating point type
// V is an arbitrary type that can be multiplied with T and added

pub struct LinearSpline<T, V> {
    x: Vec<T>,
    y: Vec<V>,
}

impl<T, V> LinearSpline<T, V>
    where T: Float + Mul<V, Output=V>,
          V: Copy + Add<V, Output=V>,
{
    pub fn from_components(args: &[T], vals: &[V]) -> Self {
        Self::from_iter(
            args.iter().zip(vals).map(|(a, b)| (*a, *b)),
        )
    }

    pub fn from_points(points: &[(T, V)]) -> Self {
        Self::from_iter(
            points.iter().copied()
        )
    }

    pub fn from_iter<I: IntoIterator<Item=(T, V)>>(points: I) -> Self {
        let (x, y): (Vec<T>, Vec<V>) = points.into_iter().unzip();
        let n = x.len();

        assert!(n >= 2, "At least two data points are required");
        assert!(x.windows(2).all(|x| x[0] < x[1]), "Arguments must be strictly increasing");

        Self {
            x,
            y,
        }
    }

    pub fn args(&self) -> &[T] {
        &self.x
    }

    pub fn vals(&self) -> &[V] {
        &self.y
    }

    pub fn arg_min(&self) -> T {
        *self.x.first().unwrap()  // Ensured by construction
    }

    pub fn arg_max(&self) -> T {
        *self.x.last().unwrap()  // Ensured by construction
    }

    // Returns the interpolated value for the given argument
    // Extrapolates on out of bounds access
    pub fn value(&self, arg: T) -> V {
        let index = bisect_right(&self.x, arg);
        let scale = (arg - self.x[index])/(self.x[index + 1] - self.x[index]);

        (T::one() - scale)*self.y[index] + scale *self.y[index + 1]
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use iter_num_tools::lin_space;
    use nalgebra::vector;

    #[test]
    fn test_interpolation_f64_single() {
        // Make sure that two control points with zero second derivative produce a straight line
        let x0 = 1.0;
        let y0 = 0.5;
        let x1 = 5.0;
        let y1 = 2.5;

        let spline = LinearSpline::from_points(&vec![(x0, y0), (x1, y1)]);
        for x in lin_space(x0..=x1, 100) {
            let d_ref = (y1 - y0)/(x1 - x0);
            let y_ref = y0 + (x - x0)*d_ref;

            assert_abs_diff_eq!(spline.value(x), y_ref, epsilon=1e-12);
        }
    }

    #[test]
    fn test_interpolation_svec_single() {
        // Make sure that two control points with zero second derivative produce a straight line
        let x0 = 1.0;
        let y0 = vector![0.5, 0.6];
        let x1 = 5.0;
        let y1 = vector![2.5, 2.6];

        let spline = LinearSpline::from_points(&vec![(x0, y0), (x1, y1)]);
        for x in lin_space(x0..=x1, 100) {
            let d_ref = (y1 - y0)/(x1 - x0);
            let y_ref = y0 + (x - x0)*d_ref;

            assert_abs_diff_eq!(spline.value(x), y_ref, epsilon=1e-12);
        }
    }

    #[test]
    fn test_interpolation_f64_multi() {
        let spline = LinearSpline::from_points(&vec![(0.0, 0.0), (1.0, 1.0), (2.0, 0.0), (3.0, -1.0), (4.0, 0.0)]);

        assert_abs_diff_eq!(spline.value(0.0),  0.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(0.5),  0.5, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(1.0),  1.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(1.5),  0.5, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(2.0),  0.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(2.5), -0.5, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(3.0), -1.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(3.5), -0.5, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(4.0),  0.0, epsilon=1e-12);

    }
}