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
            EigenSolverError::NonFiniteEigenvalues  => write!(f, "At least one of the computed Eigenvalues is non-finite.")?,
        }

        Ok(())
    }
}

impl std::error::Error for EigenSolverError {

}

#[derive(Copy, Clone, Debug)]
pub struct Mode {
    pub omega: f64,     // Undamped natural frequency
    pub zeta: f64       // Modal damping ratio
}

impl Mode {
    // Constructs mode information from a complex conjugated eigenvalue
    pub fn new(lambda: Complex<f64>) -> Self {
        let omega = lambda.abs();
        let zeta = -lambda.re/omega;

        Self {
            omega,
            zeta
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
    /*
    // Matrices A and B for transforming the quadratic eigenvalue problem into a linear one
    let A = stack![
        0, K;
        K, D;
    ];
    
    let B = stack![
        K,  0;
        0, -DMatrix::from_diagonal(M)
    ];
    */

    // Compute the complex eigenvalues for A*v = lambda*B*v by inverting B and solving B^(-1)*A*v = lambda*v.
    // Using a generalized eigenvalue solver would have been better, but nalgebra currently doesn't have one.

    let M_inv = DMatrix::from_diagonal(&M.map(|m| 1.0/m));
    let A = stack![
        0, DMatrix::identity(M.len(), M.len());
        -&M_inv*K, -&M_inv*D;
    ];

    let lambda = A.complex_eigenvalues();
    if !lambda.iter().cloned().all(Complex::<f64>::is_finite) {
        return Err(EigenSolverError::NonFiniteEigenvalues);
    }

    // Compute modal properties for each pair of complex conjugated eigenvalues
    let mut modes: Vec<Mode> = lambda.iter()
        .tuple_windows()
        .filter(|(l1, l2)| l1.conj().eq(l2))    // TODO: Should there be a small tolerance here? Seems to work fine though...
        .map(|(l1, _)| Mode::new(*l1))
        .collect();

    // Sort results by undamped natural frequency and return
    modes.sort_by(|a, b| a.omega.partial_cmp(&b.omega).expect("Failed to compare frequencies"));
    return Ok(modes);
}