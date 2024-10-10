use std::cmp::Ordering;
use nalgebra::{DMatrix, DVector, dvector, Matrix};

// The functions below compute numerical estimates for the derivative of functions using Ridder's method.
// They are only used in tests to verify analytical derivatives.
// The method is an adaptive method that automatically determines the finite difference step size and also
// does some clever extrapolation to increase the accuracy. It is much more accutate than standard finite
// differences, but also much more expensive to compute.

// Numerical derivative for a function that takes a scalar and returns a scalar
pub fn differentiate_1_to_1<F>(function: &mut F, origin: f64, stepsize: f64) -> (f64, f64)
    where F: FnMut(f64) -> f64
{
    let mut wrapper = |x| {
        return dvector![function(x)];
    };

    let (estimate, error) = diff_ridders(&mut wrapper, origin, stepsize, 1.4, 2.0, 15);
    return (estimate[0], error);
}

// Numerical derivative for a function that takes a scalar and returns a vector
pub fn differentiate_1_to_n<F>(function: &mut F, origin: f64, stepsize: f64) -> (DVector<f64>, f64)
    where F: FnMut(f64) -> DVector<f64>
{
    return diff_ridders(function, origin, stepsize, 1.4, 2.0, 15);
}

// Numerical derivative for a function that takes a vector and returns a scalar
pub fn differentiate_n_to_1<F>(function: &mut F, origin: &DVector<f64>, stepsize: f64) -> (DVector<f64>, f64)
    where F: FnMut(&DVector<f64>) -> f64
{
    let mut results = Vec::new();
    for i in 0..origin.len() {
        let mut wrapper = |x: f64| -> DVector<f64> {
            let mut argument = origin.clone();
            argument[i] = x;
            return dvector![function(&argument)];
        };

        results.push(diff_ridders(&mut wrapper, origin[i], stepsize, 1.4, 2.0, 15));
    }

    let estimate = DVector::from_iterator(results.len(), results.iter().map(|r|{ r.0[0] }));
    let error = results.iter()
        .map(|r|{ r.1 })
        .min_by(|a, b| a.partial_cmp(b).unwrap_or(Ordering::Equal))
        .unwrap();

    return (estimate, error);
}

// Numerical derivative for a function that takes a vector and returns a vector
pub fn differentiate_n_to_k<F>(function: &mut F, origin: &DVector<f64>, stepsize: f64) -> (DMatrix<f64>, f64)
    where F: FnMut(&DVector<f64>) -> DVector<f64>
{
    let mut columns = Vec::new();
    let mut max_error = f64::NEG_INFINITY;

    for i in 0..origin.len() {
        let mut wrapper = |x: f64| -> DVector<f64> {
            let mut argument = origin.clone();
            argument[i] = x;
            return function(&argument);
        };

        let (estimate, error) = diff_ridders(&mut wrapper, origin[i], stepsize, 1.4, 2.0, 15);
        columns.push(estimate);

        if error >= max_error {
            max_error = error;
        }
    }

    return (DMatrix::from_columns(&columns), max_error);
}

// This is the function that actually performs the calculation of the gradients using Ridder's method.
// It was ported from the Python testing utility 'derivecheck' (https://pypi.org/project/derivcheck/)
// The only thing that was changed is the way Ridders method is generalized to multiple dimensions.
// Instead of using vector norms for the error, each component is treated individually.
//
// Original default arguments:
//     con=1.4,
//     safe=2.0,
//     maxiter=15
//
fn diff_ridders<F>(function: &mut F, origin: f64, mut stepsize: f64, contract: f64, safe: f64, maxiter: usize) -> (DVector<f64>, f64)
    where F: FnMut(f64) -> DVector<f64>
{
    /*
    Estimate first-order derivative with Ridders' finite difference method.

    This implementation is based on the one from the book Numerical Recipes. The code
    is pythonized and no longer using fixed-size arrays. Also, the output of the function
    can be an array.

    Parameters
    ----------
    function : function
        The function to be differentiated.
    origin : float
        The point at which must be differentiated.
    stepsize : float
        The initial step size.
    contract : float
        The rate at which the step size is decreased (contracted). Must be larger than
        one.
    safe : float
        The safety check used to terminate the algorithm. If Errors between successive
        orders become larger than ``safe`` times the error on the best estimate, the
        algorithm stop. This happens due to round-off errors.
    maxiter : int
        The maximum number of iterations, equals the maximum number of function calls and
        also the highest polynomial order in the Neville method.

    Returns
    -------
    estimate : float
        The best estimate of the first-order derivative.
    error : float
        The (optimistic) estimate of the error on the derivative.
    */

    assert_ne!(stepsize, 0.0, "Stepsize must be nonzero");
    assert!(contract > 1.0, "Contract must be larger than one");
    assert!(safe > 1.0, "Safe must be larger than one");

    let mut table = vec![vec![(&function(origin + stepsize) - &function(origin - stepsize))/(2.0*stepsize)]];
    let mut estimate = DVector::<f64>::zeros(table[0][0].len());
    let mut error = DVector::<f64>::from_element(table[0][0].len(), f64::INFINITY);

    // Loop based on Neville's method.
    // Successive rows in the table will go to smaller stepsizes.
    // Successive columns in the table go to higher orders of extrapolation.
    for i in 1..maxiter {
        // Reduce step size.
        stepsize /= contract;

        // First-order approximation at current step size.
        table.push(vec![(&function(origin + stepsize) - &function(origin - stepsize))/(2.0*stepsize)]);

        // Compute higher-orders
        for j in 1..i+1 {
            // Compute extrapolations of various orders, requiring no new function evaluations.
            // This is a recursion relation based on Neville's method.
            let factor = contract.powi(2*j as i32);
            let current_estimate = (&table[i][j-1]*factor - &table[i-1][j-1])/(factor - 1.0);
            table[i].push(current_estimate.clone());

            // The error strategy is to compare each new extrapolation to one
            // order lower, both at the present stepsize and the previous one:
            let current_error = Matrix::inf(
                &(&table[i][j] - &table[i][j-1]).abs(),
                &(&table[i][j] - &table[i-1][j-1]).abs()
            );

            // Save improved estimates for components whose error has decreased.
            for i in 0..error.len() {
                if current_error[i] < error[i] {
                    estimate[i] = current_estimate[i];
                    error[i] = current_error[i];
                }
            }
        }

        // If the highest-order estimate is growing larger than the error on the best
        // estimate, the algorithm becomes numerically instable. Time to quit.
        if (&table[i][i] - &table[i-1][i-1]).abs() >= safe*&error {
            break;
        }
    }

    return (estimate, error.max());
}

#[cfg(test)]
mod tests {
    use nalgebra::dmatrix;
    use super::*;

    #[test]
    fn test_differentiate_1_to_1() {
        let mut function = |x: f64| -> f64 {
            return x.powi(2);
        };

        let derivative = |x: f64| -> f64 {
            return 2.0*x;
        };

        let origin = 1.0;
        let reference = derivative(origin);
        let (estimate, _error) = differentiate_1_to_1(&mut function, origin, 0.01);

        assert_abs_diff_eq!(estimate, reference, epsilon=1e-13);
    }

    #[test]
    fn test_differentiate_1_to_n() {
        let mut function = |x: f64| -> DVector<f64> {
            return dvector![x.powi(2), x.powi(3)];
        };

        let derivative = |x: f64| -> DVector<f64> {
            return dvector![2.0*x, 3.0*x];
        };

        let origin = 1.0;
        let reference = derivative(origin);
        let (estimate, _error) = differentiate_1_to_n(&mut function, origin, 0.01);

        assert_abs_diff_eq!(estimate, reference, epsilon=1e-13);
    }

    #[test]
    fn test_differentiate_n_to_1() {
        let mut function = |x: &DVector<f64>| -> f64 {
            return x[0].powi(2) + x[1].powi(3);
        };

        let derivative = |x: &DVector<f64>| -> DVector<f64> {
            return dvector![2.0*x[0], 3.0*x[1]];
        };

        let origin = dvector![1.0, 1.0];
        let reference = derivative(&origin);
        let (estimate, _error) = differentiate_n_to_1(&mut function, &origin, 0.01);

        assert_abs_diff_eq!(estimate, reference, epsilon=1e-13);
    }

    #[test]
    fn test_differentiate_n_to_k() {
        let mut function = |x: &DVector<f64>| -> DVector<f64> {
            return dvector![
                x[0].powi(2) + x[1].powi(3),
                x[0].powi(4) + x[1].powi(5)
            ];
        };

        let derivative = |x: &DVector<f64>| -> DMatrix<f64> {
            return dmatrix![
                2.0*x[0], 3.0*x[1];
                4.0*x[0], 5.0*x[1];
            ];
        };

        let origin = dvector![1.0, 1.0];
        let reference = derivative(&origin);
        let (estimate, _error) = differentiate_n_to_k(&mut function, &origin, 0.01);

        assert_abs_diff_eq!(estimate, reference, epsilon=1e-12);
    }
}