use nalgebra::SMatrix;

// Integrates the given function using the adaptive Simpson method (https://en.wikipedia.org/wiki/Adaptive_Simpson%27s_method) with a given numerical tolerance and maximum recursion depth.
// Retrurns either the value of the integral or None if the specified tolerance was not reached.
// Implementation based on the Wikipedia example.
#[allow(dead_code)]
pub fn adaptive_simpson<F, const R: usize, const C: usize>(mut f: F, a: f64, b: f64, epsilon: f64, max_recursion: u32) -> Option<SMatrix<f64, R, C>>
    where F: FnMut(f64) -> SMatrix<f64, R, C>
{
    let fa = f(a);
    let fb = f(b);

    let whole = simpson_quadrature(&mut f, a, b, fa, fb);
    let result = simpson_quadrature_recursive(&mut f, a, b, fa, fb, whole, epsilon, max_recursion)?;

    return if a <= b {
        Some(result)
    } else {
        Some(-result)
    }
}

struct Triple<const R: usize, const C: usize> {
    m: f64,
    fm: SMatrix<f64, R, C>,
    result: SMatrix<f64, R, C>,
}

fn simpson_quadrature_recursive<F, const R: usize, const C: usize>(f: &mut F, a: f64, b: f64, fa: SMatrix<f64, R, C>, fb: SMatrix<f64, R, C>, whole: Triple<R, C>, epsilon: f64, n: u32) -> Option<SMatrix<f64, R, C>>
    where F: FnMut(f64) -> SMatrix<f64, R, C>,
{
    if n == 0 {
        return None;
    }

    let left = simpson_quadrature(f, a, whole.m, fa, whole.fm);
    let right = simpson_quadrature(f, whole.m, b, whole.fm, fb);
    let delta = left.result + right.result - whole.result;

    if delta.amax() <= 15.0*epsilon {
        return Some(left.result + right.result + delta/15.0);
    }
    else {
        return Some(
            simpson_quadrature_recursive(f, a, whole.m, fa, whole.fm, left, epsilon/2.0, n-1)?
                + simpson_quadrature_recursive(f, whole.m, b, whole.fm, fb, right, epsilon/2.0, n-1)?
        );
    }
}

fn simpson_quadrature<F, const R: usize, const C: usize>(f: &mut F, a: f64, b: f64, fa: SMatrix<f64, R, C>, fb: SMatrix<f64, R, C>) -> Triple<R, C>
    where F: FnMut(f64) -> SMatrix<f64, R, C>,
{
    let m = (a + b)/2.0;
    let fm = f(m);
    let result = (b - a).abs()/6.0*(fa + 4.0*fm + fb);

    Triple {
        m,
        fm,
        result
    }
}

// Integrates the given function using the simpson rule and a fixed number of sample points
// Implementation based on http://camillecarvalho.org/math-131/lectures/lecture_11.html
// Has no error control but also can't fail as a result.
pub fn fixed_simpson<F, const R: usize, const C: usize>(mut f: F, a: f64, b: f64, n: usize) -> SMatrix<f64, R, C>
    where F: FnMut(f64) -> SMatrix<f64, R, C>,
{
    assert!(n.is_multiple_of(2), "Number of integration intervals must be even");

    let h = (b - a)/(n as f64);
    let mut r = f(a) + f(b);

    for i in (1..n).step_by(2) {
        r += 4.0*f(a + (i as f64)*h);
    }

    for i in (2..n-1).step_by(2) {
        r += 2.0*f(a + (i as f64)*h);
    }

    return r*h/3.0;
}

// Cumulatively integrates a series of data points by using a variant of the Simpson's rule as described in [1] and also used in SciPy [2], [3].
// [1] Cartwright, Kenneth V. Simpson’s Rule Cumulative Integration with MS Excel and Irregularly-spaced Data. Journal of Mathematical Sciences and Mathematics Education. 12 (2): 1-9
// [2] https://docs.scipy.org/doc/scipy-1.15.0/reference/generated/scipy.integrate.cumulative_simpson.html
// [3] https://github.com/scipy/scipy/blob/7372d710ef6e97a3a9f3027efcf6b5b77bb33777/scipy/integrate/_quadrature.py#L612
pub fn cumulative_simpson(x: &[f64], y: &[f64]) -> Vec<f64> {
    assert_eq!(x.len(), y.len(), "Inputs must have the same length");

    // Zero input points: Return empty vector
    if x.is_empty() {
        return vec![];
    }

    // One input point: Integral up to that point is zero
    if x.len() == 1 {
        return vec![0.0];
    }

    // Two input points: Use trapezoidal rule since Simpson's rule needs at least three points
    if x.len() == 2 {
        return vec![0.0, 0.5*(x[1] - x[0])*(y[1] + y[0])];
    }

    // Given three points at x1, x2, x3, calculates the partial integral from x1 to point x2 according to Simpson's rule
    let I12 = |(x1, y1), (x2, y2), (x3, y3)| -> f64 {
        let x12: f64 = x2 - x1;
        let x23: f64 = x3 - x2;
        let x13: f64 = x3 - x1;

        let c1 = 3.0 - x12/x13;
        let c2 = 3.0 + x12.powi(2)/(x23*x13) + x12/x13;
        let c3 = -x12.powi(2)/(x23*x13);

        x12/6.0*(c1*y1 + c2*y2 + c3*y3)
    };

    // Given three points at x1, x2, x3, calculates the full integral from point x1 to point x3 according to Simpson's rule
    let I13 = |(x1, y1), (x2, y2), (x3, y3)| {
        let x12: f64 = x2 - x1;
        let x23: f64 = x3 - x2;
        let x13: f64 = x3 - x1;

        let c1 = 2.0 - x23/x12;
        let c2 = x13.powi(2)/(x23*x12);
        let c3 = 2.0 - x12/x23;

        x13/6.0*(c1*y1 + c2*y2 + c3*y3)
    };

    // As many output values as there are input points, initialized to zero
    let mut I = vec![0.0; x.len()];

    // The integral for the first point remains zero
    // The integral for the second point is the partial integral from x[0] to x[1] of the three points x[0], x[1], x[2]
    I[1] = I12((x[0], y[0]), (x[1], y[1]), (x[2], y[2]));

    // The integral for each subsequent point k is equal to the integral two points before (k-2)
    // plus the full Simpson integral from k-2 to k.
    for k in 2..x.len() {
        I[k] = I[k-2] + I13((x[k-2], y[k-2]), (x[k-1], y[k-1]), (x[k], y[k]));
    }

    return I;
}

#[cfg(test)]
mod tests {
    use approx::assert_abs_diff_eq;
    use super::*;
    use assert2::assert;
    use nalgebra::vector;

    #[test]
    fn test_adaptive_simpson() {
        let f = |x: f64| { vector![x, x.powi(3)] };
        let a = 0.0;
        let b = 1.0;

        let F_num = adaptive_simpson(f, a, b, 1e-8, 10).expect("Integration failed");
        let F_ref = vector![0.5, 0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);

        let F_num = adaptive_simpson(f, b, a, 1e-8, 10).expect("Integration failed");
        let F_ref = vector![-0.5, -0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);
    }

    #[test]
    fn test_fixed_simpson() {
        let f = |x: f64| { vector![x, x.powi(3)] };
        let a = 0.0;
        let b = 1.0;

        let F_num = fixed_simpson(f, a, b, 100);
        let F_ref = vector![0.5, 0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);

        let F_num = fixed_simpson(f, b, a, 100);
        let F_ref = vector![-0.5, -0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);
    }

    #[test]
    fn test_cumulative_simpson() {
        // Empty input -> empty output
        let x = [];
        let y = [];
        let I = cumulative_simpson(&x, &y);
        assert!(I.is_empty());

        // One point as input -> one zero as output
        let x = [0.0];
        let y = [1.0];
        let I = cumulative_simpson(&x, &y);
        assert!(I == vec![0.0]);

        // Two points as input -> result is zero for first point and trapezoidal area for second
        let x = [0.1, 0.9];
        let y = [0.2, 0.8];
        let I = cumulative_simpson(&x, &y);
        assert!(I == vec![0.0, 0.4]);

        // Example x^2 with reference results from paper
        let x_ref = [0.0, 0.1, 0.19, 0.33, 0.4, 0.55, 0.69, 0.74, 0.9];
        let y_ref = [0.0, 0.01, 0.0361, 0.1089, 0.16, 0.3025, 0.4761, 0.5476, 0.81];
        let I_ref = [0.0, 0.000333, 0.002286, 0.011979, 0.021333, 0.055458, 0.109503, 0.135075, 0.243000];
        let I_num = cumulative_simpson(&x_ref, &y_ref);
        for i in 0..x_ref.len() {
            assert_abs_diff_eq!(I_num[i], I_ref[i], epsilon=1e-6);
        }

        // Example sin(x) with reference results from paper
        let x_ref = [0.0, 0.1, 0.19, 0.33, 0.4, 0.55, 0.69, 0.74, 0.9];
        let y_ref = [0.0, 0.099833, 0.188859, 0.324043, 0.389418, 0.522687, 0.636537, 0.674288, 0.783327];
        let I_ref = [0.0, 0.00499971, 0.01799672, 0.05395332, 0.07894859, 0.14746047, 0.22876683, 0.26152327, 0.37839294];
        let I_num = cumulative_simpson(&x_ref, &y_ref);
        for i in 0..x_ref.len() {
            assert_abs_diff_eq!(I_num[i], I_ref[i], epsilon=1e-6);
        }
    }
}