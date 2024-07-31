use nalgebra::SMatrix;

// Integrates the given function using the adaptive Simpson method (https://en.wikipedia.org/wiki/Adaptive_Simpson%27s_method) with a given numerical tolerance and maximum recursion depth.
// Retrurns either the value of the integral or None if the specified tolerance was not reached.
// Implementation based on the Wikipedia example.
pub fn integrate_adaptive<F, const R: usize, const C: usize>(mut f: F, a: f64, b: f64, epsilon: f64, max_recursion: u32) -> Option<SMatrix<f64, R, C>>
    where F: FnMut(f64) -> SMatrix<f64, R, C>,
{
    //assert!(b >= a, "Integral bounds must be in ascending order");  // TODO: Reenable after solving numerical imperfections between evaluation points

    let fa = f(a);
    let fb = f(b);

    let whole = simpson_quadrature(&mut f, a, b, fa, fb);
    simpson_quadrature_recursive(&mut f, a, b, fa, fb, whole, epsilon, max_recursion)
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

// Integrates the given function using the trapezoidal rule and a fixed number of sample points
pub fn integrate_fixed<F, const R: usize, const C: usize>(mut f: F, a: f64, b: f64, n: usize) -> Option<SMatrix<f64, R, C>>
    where F: FnMut(f64) -> SMatrix<f64, R, C>,
{
    //assert!(b >= a, "Integral bounds must be in ascending order");  // TODO: Reenable after solving numerical imperfections between evaluation points

    let h = (b - a)/(n as f64);

    let I1: SMatrix<f64, R, C> = 0.5*(f(a) + f(b));
    let I2: SMatrix<f64, R, C> = (1..=n-1).map(|i| f(a + (i as f64)*h)).sum();

    Some(h*(I1 + I2))

    //Some(h*(0.5*f(a) + 0.5*f(b) + (1..=n-1).map(|i| f(a + (i as f64)*h)).sum()))
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
    }

    #[test]
    fn test_trapezoidal_rule() {
        let f = |x: f64| { vector![x, x.powi(3)] };
        let a = 0.0;
        let b = 1.0;

        let F_num = integrate_fixed(f, a, b, 10000).expect("Integration failed");
        let F_ref = vector![0.5, 0.25];

        assert_abs_diff_eq!(F_num, F_ref, epsilon=1e-8);
    }
}