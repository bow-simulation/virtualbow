//use std::collections::VecDeque;
use std::fmt::{Display, Formatter};
use nalgebra::{DMatrix, DVector};

#[derive(Clone)]
pub struct NewtonTolerances {
    pub xtol: DVector<f64>,          // Absolute tolerances on the displacement increments
    pub λtol: f64,                   // Absolute tolerance on the load factor increments
}

impl NewtonTolerances {
    // Tolerances with constant scaling, i.e. same tolerance for each component
    pub fn constant(n: usize, xtol: f64, λtol: f64) -> Self {
        Self {
            xtol: DVector::from_element(n, xtol),
            λtol,
        }
    }
}

#[derive(Copy, Clone, Debug)]
pub struct LineSearchSettings {
    pub backtracking_window: usize,    // Number of previous errors to use for comparison
    pub backtracking_factor: f64,      // Reduction factor of the step size when backtracking
}

#[derive(Copy, Clone, Debug)]
pub struct NewtonSettings {
    pub max_iterations: usize,                        // Maximum number of iterations per load step
    pub line_searching: Option<LineSearchSettings>    // Settings for line searching if enabled
}

impl Default for NewtonSettings {
    fn default() -> Self {
        Self {
            max_iterations: 50,
            line_searching: None
        }
    }
}

impl Default for LineSearchSettings {
    fn default() -> Self {
        Self {
            backtracking_window: 5,
            backtracking_factor: 0.5,
        }
    }
}

#[derive(Clone, Default, Debug)]
pub struct NewtonInfo {
    #[allow(dead_code)]
    pub x: DVector<f64>,     // Solution variables
    #[allow(dead_code)]
    pub λ: f64,              // Solution parameter
    pub dxdλ: DVector<f64>,  // Derivative of the solution wrt. the parameter
    pub iterations: usize,     // Number of iterations that have been performed
}

#[derive(PartialEq, Debug, Copy, Clone)]
pub enum NewtonError {
    LinearSolutionFailed,     // Decomposition of the tangent stiffness matrix or solution of the linear system failed
    NonFiniteConstraintEval,  // The return value of the constraint function is not finite, i.e. contains NaN or Inf values
    NonFiniteStateIncrement,  // The displacement delta is not finite, i.e. contains NaN or Inf values
    MaxIterationsReached,     // Maximum number of iterations was reached without convergence
}

impl Display for NewtonError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            NewtonError::LinearSolutionFailed    => write!(f, "Decomposition/solution of the tangent stiffness matrix failed.")?,
            NewtonError::NonFiniteConstraintEval => write!(f, "Encountered a non-finite constraint function return value.")?,
            NewtonError::NonFiniteStateIncrement => write!(f, "Encountered a non-finite displacement increment.")?,
            NewtonError::MaxIterationsReached    => write!(f, "Maximum number of iterations exceeded.")?,
        }

        Ok(())
    }
}

pub fn solve_newton<F>(function: &mut F, x0: &DVector<f64>, tolerances: &NewtonTolerances, settings: &NewtonSettings) -> Result<NewtonInfo, NewtonError>
    where F: FnMut(&DVector<f64>, &mut DVector<f64>, &mut DMatrix<f64>)  // x -> f, dfdx
{
    // Quantities at the current solution candidate
    let mut x_current = x0.clone();
    let mut f_current = DVector::<f64>::zeros(x_current.len());
    let mut J_current = DMatrix::<f64>::zeros(x_current.len(), x_current.len());

    // Quantities at the next solution candidate
    let mut x_next = DVector::<f64>::zeros(x_current.len());
    let mut f_next = DVector::<f64>::zeros(x_current.len());
    let mut J_next = DMatrix::<f64>::zeros(x_current.len(), x_current.len());

    // Queue for keeping track of some of the previous error norms as defined by the window setting
    //let mut prev_norms = VecDeque::with_capacity(settings.backtracking_window);

    // Evaluate function and jacobian at initial solution candidate
    function(&x_current, &mut f_current, &mut J_current);

    // Residual tolerances from initial jacobian and tolerances on x
    //let ftol = J_current.diagonal().component_mul(&tolerances.xtol).abs();

    for i in 0..=settings.max_iterations {
        /*
        // Evaluate convergence criterion on residual norm, return early if fulfilled
        let norm_current = f_current.component_div(&ftol).norm_squared();
        if  norm_current < 1.0 {
            return Ok(NewtonInfo {
                x: x_current,
                λ: 0.0,
                dxdλ: DVector::zeros(0),
                iterations: i
            });
        }
        */

        // Factorize jacobian matrix and calculate solution increment
        let decomposition = J_current.clone().lu();
        let delta_x = -decomposition.solve(&f_current).ok_or(NewtonError::LinearSolutionFailed)?;

        // Check if the solution increment is finite
        if !delta_x.iter().cloned().all(f64::is_finite) {
            return Err(NewtonError::NonFiniteStateIncrement);
        }

        // Evaluate convergence criterion on solution increments, return if fulfilled
        if  delta_x.component_div(&tolerances.xtol).amax() < 1.0 {
            return Ok(NewtonInfo {
                x: x_current,
                λ: 0.0,
                dxdλ: DVector::zeros(0),
                iterations: i
            });
        }

        /*
        // Record the current squared error norm in the queue
        if prev_norms.len() == settings.backtracking_window {
            prev_norms.pop_front();
        }
        prev_norms.push_back(norm_current);
        */

        // Compute next solution candidate, perform Armijo backtracking if required
        let /*mut*/ alpha = 1.0;    // Step size to be refined by backtracking
        //loop {
            // Evaluate next solution candidate from direction (delta x) and step size (alpha)
            // Note: Evaluating J during backtracking might seem wasteful, since it is only used when the step is accepted.
            // But since we consider backtracking the exceptional case and acceptance the norm, we err on the side of computing J directly.
            // It was also easier to implement.
            x_next.copy_from(&(&x_current + alpha*&delta_x));
            function(&x_next, &mut f_next, &mut J_next);

            // Armijo condition is fulfilled: Accept the solution candidate as the current one
            // Otherwise reduce step size and try again
            /*
            let norm_next = f_next.component_div(&ftol).norm_squared();
            let slope_next = 2.0*(f_next.component_div(&ftol).component_div(&ftol)).dot(&(&J_next*&delta_x));
            let slope_current = 2.0*(f_current.component_div(&ftol).component_div(&ftol)).dot(&(&J_current*&delta_x));
            */

            /*
            // Only consider backtrack after the window of previous errors is filled
            if prev_norms.len() == settings.backtracking_window {
                // Compute the maximum previous error within the window
                let window_max = prev_norms.iter().copied().fold(f64::NEG_INFINITY, f64::max);    // Unwrap is okay because we pushed at least one element

                // If the current norm is larger than the window maximum,
                // perform backtracking and enter another loop.
                if norm_next > window_max {
                    alpha *= settings.backtracking_factor;
                    continue;
                }
            }
            */

            // Continue Newton iterations and break out of the backtracking loop
            x_current.copy_from(&x_next);
            f_current.copy_from(&f_next);
            J_current.copy_from(&J_next);
            //break;
        //}
    }

    // Maximum number of iterations exceeded
    Err(NewtonError::MaxIterationsReached)
}

// Function: x, λ -> f, dfdx, dfdλ
// Constraint: x, λ -> c, dcdx, dcdλ
pub fn solve_newton_constrained<F, C>(function: &mut F, constraint: &mut C, x0: DVector<f64>, λ0: f64, tolerances: &NewtonTolerances, settings: &NewtonSettings) -> Result<NewtonInfo, NewtonError>
    where F: FnMut(&DVector<f64>, f64, &mut DVector<f64>, &mut DMatrix<f64>, &mut DVector<f64>),
          C: FnMut(&DVector<f64>, f64, &mut f64, &mut DVector<f64>, &mut f64),
{
    let mut x_current = x0;
    let mut λ_current = λ0;
    let mut f_current = DVector::<f64>::zeros(x_current.len());
    let mut c_current = 0.0;

    let mut dfdx_current = DMatrix::<f64>::zeros(x_current.len(), x_current.len());
    let mut dfdλ_current = DVector::<f64>::zeros(x_current.len());
    let mut dcdx_current = DVector::<f64>::zeros(x_current.len());
    let mut dcdλ_current = 0.0;

    let mut x_next = DVector::<f64>::zeros(x_current.len());
    let mut λ_next;
    let mut f_next = DVector::<f64>::zeros(x_current.len());
    let mut c_next = 0.0;

    let mut dfdx_next = DMatrix::<f64>::zeros(x_current.len(), x_current.len());
    let mut dfdλ_next = DVector::<f64>::zeros(x_current.len());
    let mut dcdx_next = DVector::<f64>::zeros(x_current.len());
    let mut dcdλ_next = 0.0;

    // Queue for keeping track of some of the previous error norms as defined by the window setting
    //let mut prev_norms = VecDeque::with_capacity(settings.backtracking_window);

    // Evaluate functions and jacobians at initial solution candidate
    function(&x_current, λ_current, &mut f_current, &mut dfdx_current, &mut dfdλ_current);
    constraint(&x_current, λ_current, &mut c_current, &mut dcdx_current, &mut dcdλ_current);

    /*
    // Compute residual tolerances from initial jacobian and tolerances on x
    let ftol = (&dfdx_current*&tolerances.xtol).abs();
    let ctol = f64::abs(dcdλ_current*tolerances.λtol);
    println!("dfdx = {}", dfdx_current);
    println!("dcdλ = {}", dcdλ_current);
    println!("ftol = {}", ftol);
    println!("ctol = {}", ctol);
    */

    for i in 1..=settings.max_iterations {
        // Factorize jacobian and calculate auxiliary vectors alpha and beta
        // TODO: This could come after evaluating the convergence criterion, if it weren't for the return of beta
        let decomposition = dfdx_current.clone().lu();
        let alpha = -decomposition.solve(&f_current).ok_or(NewtonError::LinearSolutionFailed)?;
        let beta = -decomposition.solve(&dfdλ_current).ok_or(NewtonError::LinearSolutionFailed)?;

        /*
        // Evaluate convergence criterion on residual norm, return early if fulfilled
        let norm_current = f_current.component_div(&ftol).norm_squared() + (c_current/ctol).powi(2);
        if norm_current < 1.0 {
            return Ok(NewtonInfo {
                x: x_current,
                λ: λ_current,
                dxdλ: beta,
                iterations: i,
            });
        }
        */

        // Calculate delta directions for load parameter and displacements
        let delta_λ = -(c_current + dcdx_current.dot(&alpha))/(dcdλ_current + dcdx_current.dot(&beta));
        let delta_x = &alpha + &beta*delta_λ;

        // Check if the load factor increment is finite
        // The denominator might have been zero, pointing to an ill-posed problem
        if !f64::is_finite(delta_λ) {
            return Err(NewtonError::NonFiniteConstraintEval);
        }

        // Check if the displacement increment is finite
        if !delta_x.iter().cloned().all(f64::is_finite) {
            return Err(NewtonError::NonFiniteStateIncrement);
        }

        // Evaluate convergence criterion on both solution increments, return if fulfilled
        if delta_x.component_div(&tolerances.xtol).amax() < 1.0 && (delta_λ/tolerances.λtol).abs() < 1.0 {
            return Ok(NewtonInfo {
                x: x_current,
                λ: λ_current,
                dxdλ: beta,
                iterations: i,
            });
        }

        /*
        // Record the current squared error norm in the queue
        if prev_norms.len() == settings.backtracking_window {
            prev_norms.pop_front();
        }
        prev_norms.push_back(norm_current);
        */

        // Compute next solution candidate, perform Armijo backtracking if required
        let /*mut*/ alpha = 1.0;    // Step size to be refined by backtracking
        //loop {
            // Evaluate next solution candidate from directions and step size (alpha)
            // Note: Evaluating jacobians during backtracking might seem wasteful, since it is only used when the step is accepted.
            // But since we consider backtracking the exceptional case and acceptance the norm, we err on the side of computing J directly.
            // It was also easier to implement.
            x_next.copy_from(&(&x_current + alpha*&delta_x));
            λ_next = λ_current + alpha*delta_λ;
            function(&x_next, λ_next, &mut f_next, &mut dfdx_next, &mut dfdλ_next);
            constraint(&x_next, λ_next, &mut c_next, &mut dcdx_next, &mut dcdλ_next);

            // Armijo condition is fulfilled or we're close to the root: Accept the solution candidate as the current one and continue
            // Otherwise reduce step size and try again
            /*
            let norm_next = f_next.component_div(&ftol).norm_squared() + (c_next/ctol).powi(2);
            let slope = 2.0*(f_current.component_div(&ftol).component_div(&ftol)).dot(&(&dfdx_current*&delta_x + &dfdλ_current*delta_λ))
                + 2.0*c_current/(ctol*ctol)*(dcdx_current.dot(&delta_x) + dcdλ_current*delta_λ);
            */

            /*
            // Only consider backtrack after the window of previous errors is filled
            if prev_norms.len() == settings.backtracking_window {
                // Compute the maximum previous error within the window
                let window_max = prev_norms.iter().copied().fold(f64::NEG_INFINITY, f64::max);    // Unwrap is okay because we pushed at least one element

                // If the current norm is larger than the window maximum,
                // perform backtracking and enter another loop.
                if norm_next > window_max {
                    alpha *= settings.backtracking_factor;
                    continue;
                }
            }
            */

            // Continue Newton iterations and break out of the backtracking loop

            x_current.copy_from(&x_next);
            f_current.copy_from(&f_next);
            λ_current = λ_next;
            c_current = c_next;

            dfdx_current.copy_from(&dfdx_next);
            dfdλ_current.copy_from(&dfdλ_next);
            dcdx_current.copy_from(&dcdx_next);
            dcdλ_current = dcdλ_next;

            //break;
        //}
    }

    // Maximum number of iterations exceeded
    Err(NewtonError::MaxIterationsReached)
}

#[cfg(test)]
mod tests {
    use approx::assert_abs_diff_eq;
    use assert2::assert;
    use nalgebra::{DMatrix, DVector, dvector};
    use crate::utils::newton::{NewtonSettings, solve_newton, solve_newton_constrained, NewtonTolerances};

    #[test]
    fn test_unconstrained() {
        // Example function from Wikipedia [1], including reference solution for each iteration.
        // https://en.wikipedia.org/wiki/Newton%27s_method#Example

        let mut x_num = Vec::new();
        let x_ref = [
            dvector![1.0, 1.0],
            dvector![0.617789, -0.279818],
            dvector![0.568334, -0.312859],
            dvector![0.567305, -0.309435],
            dvector![0.567297, -0.309442]
        ];

        let mut f = |x: &DVector<f64>, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>| {
            x_num.push(x.clone());

            f[0] = 5.0*x[0].powi(2) + x[0]*x[1].powi(2) + f64::sin(2.0*x[1]).powi(2) - 2.0;
            f[1] = f64::exp(2.0*x[0] - x[1]) + 4.0*x[1] - 3.0;

            dfdx[(0, 0)] = 10.0*x[0] + x[1].powi(2);
            dfdx[(0, 1)] = 2.0*x[0]*x[1] + 4.0*f64::sin(2.0*x[1])*f64::cos(2.0*x[1]);
            dfdx[(1, 0)] = 2.0*f64::exp(2.0*x[0] - x[1]);
            dfdx[(1, 1)] = -f64::exp(2.0*x[0] - x[1]) + 4.0;
        };

        let x0 = dvector![1.0, 1.0];
        let result = solve_newton(&mut f, &x0, &NewtonTolerances::constant(2, 1e-6, 1e-6), &NewtonSettings::default()).unwrap();

        // Check of the solution converged in the same number of steps as the reference,
        // which depends on the numerical tolerances.
        assert!(x_num.len() == x_ref.len());

        // Check if the computed intermediate solutions match the reference
        for i in 0..x_ref.len() {
            assert_abs_diff_eq!(x_num[i], x_ref[i], epsilon=1e-6);
        }

        // Check if the result contains the same same solution that the function was called last with
        assert!(result.x == *x_num.last().unwrap());
    }

    #[test]
    fn test_constrained_1() {
        let mut x_num = Vec::new();

        // Example function from Wikipedia: https://en.wikipedia.org/wiki/Newton%27s_method#Example
        // Modified by a parameter λ that scales the constant offsets
        let mut f = |x: &DVector<f64>, λ: f64, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>, dfdλ: &mut DVector<f64>| {
            x_num.push(x.clone());

            f[0] = 5.0*x[0].powi(2) + x[0]*x[1].powi(2) + f64::sin(2.0*x[1]).powi(2) - 2.0*λ.powi(2);
            f[1] = f64::exp(2.0*x[0] - x[1]) + 4.0*x[1] - 3.0*λ;

            dfdx[(0, 0)] = 10.0*x[0] + x[1].powi(2);
            dfdx[(0, 1)] = 2.0*x[0]*x[1] + 4.0*f64::sin(2.0*x[1])*f64::cos(2.0*x[1]);
            dfdx[(1, 0)] = 2.0*f64::exp(2.0*x[0] - x[1]);
            dfdx[(1, 1)] = -f64::exp(2.0*x[0] - x[1]) + 4.0;

            dfdλ[0] = -4.0*λ;
            dfdλ[1] = -3.0;
        };

        // Constraint function 1: Constrain the first solution component to the actual solution of the original function.
        // The resulting solution variables must be the same as before and lambda must be 1.
        let mut c1 = |x: &DVector<f64>, _λ: f64, c: &mut f64, dcdx: &mut DVector<f64>, dcdλ: &mut f64| {
            *c = x[0] - 0.567297;
            *dcdλ = 0.0;

            dcdx[0] = 1.0;
            dcdx[1] = 0.0;
        };

        // Constraint function 2: Constrain lambda to be 1. Again, the reference solution of the unmodified function must appear.
        let mut c2 = |_x: &DVector<f64>, λ: f64, c: &mut f64, dcdx: &mut DVector<f64>, dcdλ: &mut f64| {
            *c = λ - 1.0;
            *dcdλ = 1.0;

            dcdx[0] = 0.0;
            dcdx[1] = 0.0;
        };

        let x0 = dvector![1.0, 1.0];
        let λ0 = 0.9;

        // Check results for constraint function 1
        let result = solve_newton_constrained(&mut f, &mut c1, x0.clone(), λ0, &NewtonTolerances::constant(2, 1e-6, 1e-6), &NewtonSettings::default()).unwrap();
        assert_abs_diff_eq!(result.x, dvector![0.567297, -0.309442], epsilon=1e-6);
        assert_abs_diff_eq!(result.λ, 1.0, epsilon=1e-5);

        // Check results for constraint function 2
        let result = solve_newton_constrained(&mut f, &mut c2, x0.clone(), λ0, &NewtonTolerances::constant(2, 1e-6, 1e-6), &NewtonSettings::default()).unwrap();
        assert_abs_diff_eq!(result.x, dvector![0.567297, -0.309442], epsilon=1e-6);
        assert_abs_diff_eq!(result.λ, 1.0, epsilon=1e-5);

        // Check if the result contains the same same solution that the function was called last with
        assert!(&result.x == x_num.last().unwrap());
    }

    #[test]
    fn test_constrained_2() {
        // Use a more simple example function with an analytical solution so that the derivative dx/dλ of the solution can be verified.
        // Inspired by example from https://www.youtube.com/watch?v=AK2APTz1ZLA
        //
        // System of two equations (representing the intersection of a line and a parabola):
        //
        // x0 - x1^2 + 8*λ = 0
        // x0 - 2*x1 + 5*λ = 0
        //
        // One of the solutions is (picked the positive square root):
        //
        // x[1] = 1 + sqrt(3*λ + 1)
        // x[0] = 2*x[1] - 5*λ
        //
        // Therefore the derivative wrt λ is:
        //
        // d/dλ x[1] = 3/(2*sqrt(3*λ + 1))
        // d/dλ x[0] = 2*(d/dλ x[1]) - 5

        let mut f = |x: &DVector<f64>, λ: f64, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>, dfdλ: &mut DVector<f64>| {
            f[0] = x[0] - x[1].powi(2) + 8.0*λ;
            f[1] = x[0] - 2.0*x[1] + 5.0*λ;

            dfdx[(0, 0)] = 1.0;
            dfdx[(0, 1)] = -2.0*x[1];
            dfdx[(1, 0)] = 1.0;
            dfdx[(1, 1)] = -2.0;

            dfdλ[0] = 8.0;
            dfdλ[1] = 5.0;
        };

        // The constant function just fixes the parameter at λ = 1
        let mut c = |_x: &DVector<f64>, λ: f64, c: &mut f64, dcdx: &mut DVector<f64>, dcdλ: &mut f64| {
            *c = λ - 1.0;
            *dcdλ = 1.0;

            dcdx[0] = 0.0;
            dcdx[1] = 0.0;
        };

        // Reference solution for λ = 1:
        let x_ref = dvector![1.0, 3.0];
        let λ_ref = 1.0;
        let dxdλ_ref = dvector![-3.5, 0.75];

        // Perform solution at a starting point that converges against the solution we picked previously,
        // compare the solution and its derivative to the parameter λ against the analytical solution for λ = 1.
        let result = solve_newton_constrained(&mut f, &mut c, dvector![2.5, 1.5], 1.2, &NewtonTolerances::constant(2, 1e-6, 1e-6), &NewtonSettings::default()).unwrap();
        assert_abs_diff_eq!(result.x, x_ref, epsilon=1e-6);
        assert_abs_diff_eq!(result.λ, λ_ref, epsilon=1e-6);
        assert_abs_diff_eq!(result.dxdλ, dxdλ_ref, epsilon=1e-6);
    }
}