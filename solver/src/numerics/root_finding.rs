use nalgebra::{DMatrix, DVector};

// Multi-dimensional Newton method for dynamically sized vectors
pub fn newton_method<F>(function: &F, x0: &DVector<f64>, ftol: f64, iter: usize) -> Option<DVector<f64>>
    where F: Fn(&DVector<f64>) -> (DVector<f64>, DMatrix<f64>)
{
    let mut x = x0.clone();

    for _ in 1..iter {
        let (f, dfdx) = function(&x);
        if f.norm() < ftol {
            return Some(x);
        }

        x -= dfdx.lu().solve(&f).expect("Failed to perform newton iteration");
    }

    return None;
}

// One-dimensional secant method (https://en.wikipedia.org/wiki/Secant_method)
// TODO: Remove if not actually used?
pub fn secant_method<F>(mut f: F, mut x0: f64, mut f0: f64, mut x1: f64, mut f1: f64, ftol: f64, iter: usize) -> Option<f64>
    where F: FnMut(f64) -> f64
{
    for _ in 0..iter {
        // Check for convergence
        if f1.abs() < ftol {
            return Some(x1);
        }

        // Compute next point
        let x2 = x1 - (x1 - x0)/(f1 - f0)*f1;

        // Shift previous points
        x0 = x1;
        x1 = x2;
        f0 = f1;
        f1 = f(x2);
    }

    return None;
}

// Regula Falsi with Illinois modification, adapted from example on Wikipedia (https://en.wikipedia.org/wiki/Regula_falsi)
// In this version, the function values at the initial interval bounds are supplied by the caller
// in order to avoid re-computation when they're already known.
// TODO: Have a look at Brent's method: https://en.wikipedia.org/wiki/Brent%27s_method
pub fn regula_falsi<F>(mut f: F, mut a: f64, mut b: f64, mut fa: f64, mut fb: f64, xtol: f64, ftol: f64, iter: usize) -> Option<f64>
    where F: FnMut(f64) -> f64
{
    let mut side = 0;

    for _ in 0..iter {
        // Compute new candidate and evaluate function
        let c = (fa*b - fb*a)/(fa - fb);
        let fc = f(c);

        // Check for convergence
        if (b - a).abs() < xtol || fc.abs() < ftol{
            return Some(c);
        }

        // If fc and fb have same sign, c is the new right boundary
        // If fc and fa have same sign, c is new left boundary
        if fc*fb > 0.0 {
            b = c;
            fb = fc;

            if side == -1 {
                fa /= 2.0;
            }

            side = -1;
        }
        else if fc*fa > 0.0 {
            a = c;
            fa = fc;

            if side == 1 {
                fb /= 2.0;
            }

            side = 1;
        }
    }

    None
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_newton_method() {
        // Tests the newton method against an example problem with solution found on stackexchange
        // https://math.stackexchange.com/a/467051
        let function = |x: &DVector<f64>| {
            let f = DVector::from_row_slice(&[
                f64::cos(    x[0]) + f64::cos(    x[1]) + f64::cos(    x[2]) - 3.0/5.0,
                f64::cos(3.0*x[0]) + f64::cos(3.0*x[1]) + f64::cos(3.0*x[2]),
                f64::cos(5.0*x[0]) + f64::cos(5.0*x[1]) + f64::cos(5.0*x[2])
            ]);

            let dfdx = DMatrix::from_row_slice(3, 3, &[
                -f64::sin(    x[0]),     -f64::sin(    x[1]),     -f64::sin(   x[2]),
                -3.0*f64::sin(3.0*x[0]), -3.0*f64::sin(3.0*x[1]), -3.0*f64::sin(3.0*x[2]),
                -5.0*f64::sin(5.0*x[0]), -5.0*f64::sin(5.0*x[1]), -5.0*f64::sin(5.0*x[2]),
            ]);

            return (f, dfdx);
        };

        let x0 = DVector::from_row_slice(&[4.0, 1.0, -0.5]);
        let x_num = newton_method(&function, &x0, 1e-8, 50).expect("Failed to find solution");
        let x_ref = DVector::from_row_slice(&[4.094047142323228, 1.2318378396357232, -0.5601417633888767]);

        assert_abs_diff_eq!(x_num, x_ref, epsilon=1e-6);
    }

    #[test]
    fn test_regula_falsi() {
        let f = |x| { f64::cos(x) - x.powi(3) };
        let a = 0.0;
        let b = 1.0;

        // Reference solution from https://en.wikipedia.org/wiki/Regula_falsi
        let x_ref = 0.865474033101614;

        let x_ab = regula_falsi(&f, a, b, f(a), f(b), 0.0, 1e-15, 50).expect("Solution failed");
        let x_ba = regula_falsi(&f, b, a, f(b), f(a), 0.0, 1e-15, 50).expect("Solution failed");

        assert_abs_diff_eq!(x_ab, x_ref, epsilon=1e-15);
        assert_abs_diff_eq!(x_ba, x_ref, epsilon=1e-15);
    }

    #[test]
    fn test_secant_method() {
        let f = |x: f64| { x.powi(3) };

        let x0 = -1.0;
        let x1 = 1.0;
        let x_num = secant_method(&f, x0, f(x0), x1, f(x1), 1e-8, 50).expect("Failed to find solution");
        let x_ref = 0.0;

        assert_abs_diff_eq!(x_num, x_ref, epsilon=1e-6);
    }
}