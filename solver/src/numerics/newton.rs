use std::fmt::{Display, Formatter};
use nalgebra::{DMatrix, DVector};

#[derive(Copy, Clone)]
pub struct Settings {
    pub epsilon_rel: f64,       // Relative tolerance
    pub epsilon_abs: f64,       // Absolute tolerance
    pub max_iterations: u32,    // Maximum number of iterations per load step
    pub max_stagnation: u32     // Maximum number of iterations that don't improve the objective
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            epsilon_rel: 1e-08,
            epsilon_abs: 1e-10,
            max_iterations: 50,
            max_stagnation: 10
        }
    }
}

#[derive(Debug)]
pub struct Info {
    pub iterations: u32,    // Number of iterations performed
}

#[derive(PartialEq, Debug, Copy, Clone)]
pub enum Error {
    LinearSolutionFailed,     // Decomposition of the tangent stiffness matrix or solution of the linear system failed
    NonFiniteStateIncrement,  // The displacement delta is not finite, i.e. contains NaN or Inf values
    MaxIterationsReached,     // Maximum number of iterations was reached without convergence
    MaxStagnationReached,     // Maximum number of iterations without improvement was reached
}

impl Display for Error {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            Error::LinearSolutionFailed    => write!(f, "Decomposition/solution of the tangent stiffness matrix failed.")?,
            Error::NonFiniteStateIncrement => write!(f, "Encountered a non-finite displacement increment.")?,
            Error::MaxIterationsReached    => write!(f, "Maximum number of iterations exceeded.")?,
            Error::MaxStagnationReached    => write!(f, "Maximum number of stagnating iterations exceeded.")?,
        }

        Ok(())
    }
}

pub fn solve_newton<F>(mut function: F, x0: DVector<f64>, settings: Settings) -> Result<Info, Error>
    where F: FnMut(&DVector<f64>, &mut DVector<f64>, &mut DMatrix<f64>)
{
    // Set velocities to zero and copy the current displacement vector
    let mut x = x0;
    let mut f = DVector::<f64>::zeros(x.len());
    let mut dfdx = DMatrix::<f64>::zeros(x.len(), x.len());

    let mut min_error_x = f64::INFINITY;
    let mut stagnations = 0_u32;

    for i in 1..=settings.max_iterations {
        // Apply state to system and elements and evaluate internal forces and tangent stiffness matrix
        function(&x, &mut f, &mut dfdx);

        // Factorize stiffness matrix, calculate out of balance loads and auxiliary vectors alpha and beta
        let decomposition = dfdx.clone().lu();
        let delta_x = decomposition.solve(&f).ok_or(Error::LinearSolutionFailed)?;

        // Check if the displacement increment is finite
        if !delta_x.iter().cloned().all(f64::is_finite) {
            return Err(Error::NonFiniteStateIncrement);
        }

        // Check convergence criteria, return if fulfilled
        let abs_error_x = delta_x.amax();
        if abs_error_x < settings.epsilon_rel*x.amax() + settings.epsilon_abs {
            return Ok(Info { iterations: i });
        }

        // Check if any of the errors has been decreased
        // If yes, record the new minimum and reset the stagnation counter
        // If no, increase the stagnation counter and return error if the limit is reached
        if abs_error_x < min_error_x {
            min_error_x = abs_error_x;
            stagnations = 0;
        }
        else {
            stagnations += 1;
            if stagnations == settings.max_stagnation {
                return Err(Error::MaxStagnationReached);
            }
        }

        // Apply changes to displacements
        x -= &delta_x;
    }

    // Maximum number of iterations exceeded
    return Err(Error::MaxIterationsReached);
}

#[cfg(test)]
mod tests {
    use nalgebra::{DMatrix, DVector, vector};
    use crate::fem::elements::string::StringElement;
    use crate::fem::system::system::System;
    use crate::numerics::newton::{Settings, solve_newton};

    #[test]
    fn test_unconstrained() {
        let a = 1.0;
        let EA = 21000.0;

        let x_ref = a + 0.001;           // Reference displacement
        let F_ref = EA/a*(x_ref - a);    // Reference force

        let mut system = System::new();
        let node1 = system.create_node(&vector![0.0, 0.0, 0.0], &[false, false, false]);
        let node2 = system.create_node(&vector![a, 0.0, 0.0], &[true, false, false]);

        system.add_element(&[node1, node2], StringElement::bar(EA, 0.0, a));
        system.add_force(node2.x(), move |_t|{ F_ref });

        let mut eval = system.create_static_eval();
        let x0 = system.get_displacements().clone();

        let f = |x: &DVector<f64>, f: &mut DVector<f64>, dfdx: &mut DMatrix<f64>| {
            system.set_displacements(&x);
            system.eval_statics(&mut eval);

            f.copy_from(&(eval.get_internal_forces() - eval.get_unscaled_external_forces()));
            dfdx.copy_from(eval.get_tangent_stiffness_matrix());
        };

        solve_newton(f, x0, Settings::default()).unwrap();

        let x_sys = system.get_displacement(node2.x());
        assert_relative_eq!(x_sys, x_ref, max_relative=1e-6);
    }
}