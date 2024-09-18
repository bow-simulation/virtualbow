use std::fmt::{Debug, Display, Formatter};
use nalgebra::SMatrix;

// Integrates the given function using the adaptive Simpson method (https://en.wikipedia.org/wiki/Adaptive_Simpson%27s_method) with a given numerical tolerance and maximum recursion depth.
// Retrurns either the value of the integral or None if the specified tolerance was not reached.
// Implementation based on the Wikipedia example.
pub fn integrate_adaptive<F, const R: usize, const C: usize>(mut f: F, a: f64, b: f64, epsilon: f64, max_recursion: u32) -> Option<SMatrix<f64, R, C>>
    where F: FnMut(f64) -> SMatrix<f64, R, C>,
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
pub fn integrate_fixed<F, const R: usize, const C: usize>(mut f: F, a: f64, b: f64, n: usize) -> SMatrix<f64, R, C>
    where F: FnMut(f64) -> SMatrix<f64, R, C>,
{
    assert!(n % 2 == 0, "Number of integration intervals must be even");

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

#[cfg(test)]
mod tests {
    use nalgebra::vector;
    use super::*;

    #[test]
    fn test_adaptive_simpson() {
        let f = |x: f64| { vector![x, x.powi(3)] };
        let a = 0.0;
        let b = 1.0;

        let F_num = integrate_adaptive(f, a, b, 1e-8, 10).expect("Integration failed");
        let F_ref = vector![0.5, 0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);

        let F_num = integrate_adaptive(f, b, a, 1e-8, 10).expect("Integration failed");
        let F_ref = vector![-0.5, -0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);
    }

    #[test]
    fn test_fixed_simpson() {
        let f = |x: f64| { vector![x, x.powi(3)] };
        let a = 0.0;
        let b = 1.0;

        let F_num = integrate_fixed(f, a, b, 100);
        let F_ref = vector![0.5, 0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);

        let F_num = integrate_fixed(f, b, a, 100);
        let F_ref = vector![-0.5, -0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);
    }
}