use std::fmt::{Display, Formatter};
use nalgebra::{DMatrix, DVector};

#[derive(Copy, Clone)]
pub struct NewtonSettings {
    pub epsilon_rel: f64,       // Relative tolerance
    pub epsilon_abs: f64,       // Absolute tolerance
    pub max_iterations: u32,    // Maximum number of iterations per load step
    pub max_stagnation: u32     // Maximum number of iterations that don't improve the objective
}

impl Default for NewtonSettings {
    fn default() -> Self {
        Self {
            epsilon_rel: 1e-4,
            epsilon_abs: 1e-6,
            max_iterations: 50,
            max_stagnation: 15,
        }
    }
}

#[derive(Debug)]
pub struct IterationResult {
    pub x: DVector<f64>,     // Solution variables
    pub λ: f64,              // Solution parameter
    pub dxdλ: DVector<f64>,  // Derivative of the solution wrt. the parameter
    pub iterations: u32,     // Number of iterations that have been performed
}

#[derive(PartialEq, Debug, Copy, Clone)]
pub enum NewtonError {
    LinearSolutionFailed,     // Decomposition of the tangent stiffness matrix or solution of the linear system failed
    NonFiniteConstraintEval,  // The return value of the constraint function is not finite, i.e. contains NaN or Inf values
    NonFiniteStateIncrement,  // The displacement delta is not finite, i.e. contains NaN or Inf values
    MaxIterationsReached,     // Maximum number of iterations was reached without convergence
    MaxStagnationReached,     // Maximum number of iterations without improvement was reached
}

impl Display for NewtonError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            NewtonError::LinearSolutionFailed    => write!(f, "Decomposition/solution of the tangent stiffness matrix failed.")?,
            NewtonError::NonFiniteConstraintEval => write!(f, "Encountered a non-finite constraint function return value.")?,
            NewtonError::NonFiniteStateIncrement => write!(f, "Encountered a non-finite displacement increment.")?,
            NewtonError::MaxIterationsReached    => write!(f, "Maximum number of iterations exceeded.")?,
            NewtonError::MaxStagnationReached    => write!(f, "Maximum number of stagnating iterations exceeded.")?,
        }

        Ok(())
    }
}

pub fn solve_newton<F>(function: &mut F, x0: DVector<f64>, settings: NewtonSettings) -> Result<IterationResult, NewtonError>
    where F: FnMut(&DVector<f64>, &mut DVector<f64>, &mut DMatrix<f64>)  // x -> f, dfdx
{
    // Set velocities to zero and copy the current displacement vector
    let mut x = x0;
    let mut f = DVector::<f64>::zeros(x.len());
    let mut dfdx = DMatrix::<f64>::zeros(x.len(), x.len());

    let mut error_ref = 0.0;              // Reference error at the first evaluation
    let mut error_min = f64::INFINITY;    // Minimum encountered error
    let mut stagnations = 0_u32;          // Current number of iterations without improvement of the minimum error

    for i in 1..=settings.max_iterations {
        // Evaluate function and jacobian at current solution
        function(&x, &mut f, &mut dfdx);

        // Factorize jacobian matrix and calculate solution increment
        let decomposition = dfdx.clone().lu();
        let delta_x = decomposition.solve(&f).ok_or(NewtonError::LinearSolutionFailed)?;

        // Check if the solution increment is finite
        if !delta_x.iter().cloned().all(f64::is_finite) {
            return Err(NewtonError::NonFiniteStateIncrement);
        }

        // Compute error of the residual
        // If first iteration, store value as reference for relative comparison
        let error = f.amax();
        if i == 1 {
            error_ref = error;
        }

        // Evaluate absolute and relative convergence criteria
        if error < settings.epsilon_abs || error/error_ref < settings.epsilon_rel {
            return Ok(IterationResult {
                x,
                λ: 0.0,
                dxdλ: DVector::zeros(0),
                iterations: i
            });
        }

        // Check if the minimum error has been decreased
        // If yes, record the new minimum and reset the stagnation counter
        // If no, increase the stagnation counter and return error if the limit is reached
        if error < error_min {
            error_min = error;
            stagnations = 0;
        }
        else {
            stagnations += 1;
            if stagnations == settings.max_stagnation {
                return Err(NewtonError::MaxStagnationReached);
            }
        }

        // Apply changes to displacements
        x -= &delta_x;

        // Remember error at first iteration as reference

    }

    // Maximum number of iterations exceeded
    return Err(NewtonError::MaxIterationsReached);
}

// Function: x, λ -> f, dfdx, dfdλ
// Constraint: x, λ -> c, dcdx, dcdλ
pub fn solve_newton_constrained<F, C>(function: &mut F, constraint: &mut C, x0: DVector<f64>, λ0: f64, settings: NewtonSettings) -> Result<IterationResult, NewtonError>
    where F: FnMut(&DVector<f64>, f64, &mut DVector<f64>, &mut DMatrix<f64>, &mut DVector<f64>),
          C: FnMut(&DVector<f64>, f64, &mut f64, &mut DVector<f64>, &mut f64),
{
    let mut error_ref_f = 0.0;              // Reference error of the function at the first evaluation
    let mut error_min_f = f64::INFINITY;    // Minimum encountered error

    let mut error_ref_c = 0.0;              // Reference error of the constraint at the first evaluation
    let mut error_min_c = f64::INFINITY;    // Minimum encountered error

    let mut stagnations = 0_u32;            // Counter for number of stagnant iterations

    let mut x = x0;
    let mut λ = λ0;
    let mut f = DVector::<f64>::zeros(x.len());
    let mut c = 0.0;

    let mut dfdx = DMatrix::<f64>::zeros(x.len(), x.len());
    let mut dfdλ = DVector::<f64>::zeros(x.len());
    let mut dcdx = DVector::<f64>::zeros(x.len());
    let mut dcdλ = 0.0;

    for i in 1..=settings.max_iterations {
        // Evaluate function and jacobian
        function(&x, λ, &mut f, &mut dfdx, &mut dfdλ);

        // Factorize jacobian and calculate auxiliary vectors alpha and beta
        let decomposition = dfdx.clone().lu();
        let alpha = -decomposition.solve(&f).ok_or(NewtonError::LinearSolutionFailed)?;
        let beta = -decomposition.solve(&dfdλ).ok_or(NewtonError::LinearSolutionFailed)?;

        // Evaluate constraint and calculate change in load parameter and displacement
        constraint(&x, λ, &mut c, &mut dcdx, &mut dcdλ);
        let delta_λ = -(c + dcdx.dot(&alpha))/(dcdλ + dcdx.dot(&beta));
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

        // Compute errors of function and constraint
        // If first iteration, store values as reference for relative comparison
        let error_f = f.amax();
        let error_c = c.abs();
        if i == 1 {
            error_ref_f = error_f;
            error_ref_c = error_c;
        }

        // Check convergence criteria, return only if both are fulfilled
        let stopping_criterion_f = error_f < settings.epsilon_abs || error_f/error_ref_f < settings.epsilon_rel;
        let stopping_criterion_c = error_c < settings.epsilon_abs || error_c/error_ref_c < settings.epsilon_rel;
        if stopping_criterion_f && stopping_criterion_c {
            return Ok(IterationResult {
                x,
                λ,
                dxdλ: beta,
                iterations: i,
            });
        }

        // Check if any of the errors has been decreased
        // If yes, record the new minimum and reset the stagnation counter
        // If no, increase the stagnation counter and return error if the limit is reached
        if error_f < error_min_f {
            error_min_f = error_f;
            stagnations = 0;
        }
        else if error_c < error_min_c {
            error_min_c = error_c;
            stagnations = 0;
        }
        else {
            stagnations += 1;
            if stagnations == settings.max_stagnation {
                return Err(NewtonError::MaxStagnationReached);
            }
        }

        // Apply changes to load factor and displacements
        x += &delta_x;
        λ += delta_λ;
    }

    // Maximum number of iterations exceeded
    return Err(NewtonError::MaxIterationsReached);
}

#[cfg(test)]
mod tests {
    use nalgebra::{DMatrix, DVector, dvector};
    use crate::numerics::newton::{NewtonSettings, solve_newton, solve_newton_constrained};

    #[test]
    fn test_unconstrained() {
        // Example function from Wikipedia [1], including reference solution for each iteration.
        // https://en.wikipedia.org/wiki/Newton%27s_method#Example

        let mut x_num = Vec::new();
        let x_ref = vec![
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
        let result = solve_newton(&mut f, x0, NewtonSettings { epsilon_rel: 1e-6, epsilon_abs: 0.0, ..Default::default() }).unwrap();

        // Check of the solution converged in the same number of steps as the reference,
        // which depends on the numerical tolerances.
        assert_eq!(x_num.len(), x_ref.len());

        // Check if the computed intermediate solutions match the reference
        for i in 0..x_ref.len() {
            assert_abs_diff_eq!(x_num[i], x_ref[i], epsilon=1e-6);
        }

        // Check if the result contains the same same solution that the function was called last with
        assert_eq!(result.x, *x_num.last().unwrap());
    }

    #[test]
    fn test_constrained() {
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
        let result = solve_newton_constrained(&mut f, &mut c1, x0.clone(), λ0, NewtonSettings { epsilon_rel: 1e-6, epsilon_abs: 0.0, ..Default::default() }).unwrap();
        assert_abs_diff_eq!(result.x, dvector![0.567297, -0.309442], epsilon=1e-6);
        assert_abs_diff_eq!(result.λ, 1.0, epsilon=1e-5);

        // Check results for constraint function 2
        let result = solve_newton_constrained(&mut f, &mut c2, x0.clone(), λ0, NewtonSettings { epsilon_rel: 1e-6, epsilon_abs: 0.0, ..Default::default() }).unwrap();
        assert_abs_diff_eq!(result.x, dvector![0.567297, -0.309442], epsilon=1e-6);
        assert_abs_diff_eq!(result.λ, 1.0, epsilon=1e-5);

        // Check if the result contains the same same solution that the function was called last with
        assert_eq!(&result.x, x_num.last().unwrap());
    }
}