use std::fmt::{Display, Formatter};
use nalgebra::{Complex, ComplexField, DMatrix, DVector, stack};
use itertools::Itertools;
use crate::fem::system::system::System;

#[derive(PartialEq, Debug)]
pub enum EigenSolverError {
    MatrixInversionFailed,
    NonFiniteEigenvalues,
}

impl Display for EigenSolverError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            EigenSolverError::MatrixInversionFailed => write!(f, "Inversion of the system matrix failed.")?,
            EigenSolverError::NonFiniteEigenvalues  => write!(f, "At least one computed Eigenvalue is non-finite.")?,
        }

        Ok(())
    }
}

impl std::error::Error for EigenSolverError {

}


#[derive(Copy, Clone, Debug)]
pub struct Mode {
    pub omega0: f64,    // Undamped frequency
    pub omega: f64,     // Damped frequency
    pub zeta: f64       // Damping ratio
}

impl Mode {
    pub fn new(lambda: Complex<f64>) -> Self {
        Self {
            omega0: lambda.abs(),
            omega: lambda.im.abs(),
            zeta: -lambda.re
        }
    }
}

// Finds the natural frequencies of the system
pub fn natural_frequencies(system: &mut System) -> Result<Vec<Mode>, EigenSolverError> {
    let mut eval = system.create_eigen_eval();
    system.eval_eigen(&mut eval);

    return natural_frequencies_from_matrices(
        &eval.get_mass_matrix(),
        &eval.get_tangent_damping_matrix(),
        &eval.get_tangent_stiffness_matrix()
    );
}

pub fn natural_frequencies_from_matrices(M: &DVector<f64>, D: &DMatrix<f64>, K: &DMatrix<f64>) -> Result<Vec<Mode>, EigenSolverError> {
    let A = stack![
        0, K;
        K, D;
    ];

    let B = stack![
        K,  0;
        0, -DMatrix::from_diagonal(M)
    ];

    // Compute the complex eigenvalues for A*v = lambda*B*v
    let B_inv = B.try_inverse().ok_or(EigenSolverError::MatrixInversionFailed)?;
    let lambda = (B_inv*A).complex_eigenvalues();

    if !lambda.iter().cloned().all(Complex::<f64>::is_finite) {
        return Err(EigenSolverError::NonFiniteEigenvalues);
    }

    // Create natural frequency result for each pair of complex conjugated eigenvalues
    let mut modes: Vec<Mode> = lambda.iter()
        .tuple_windows()
        .filter(|(l1, l2)| l1.conj().eq(l2))    // TODO: Should there be a small tolerance here?
        .map(|(l1, _)| Mode::new(*l1))
        .collect();

    // Sort results by undamped natural frequency and return result
    modes.sort_by(|a, b| a.omega0.partial_cmp(&b.omega0).expect("Failed to sort by frequency"));
    return Ok(modes);
}