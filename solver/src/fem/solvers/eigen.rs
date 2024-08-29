use nalgebra::{Complex, ComplexField, DMatrix, DVector, stack};
use itertools::Itertools;
use crate::fem::system::system::System;

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
pub fn natural_frequencies(system: &mut System) -> Vec<Mode> {
    let mut eval = system.default_eigen_eval();
    system.eval_eigen(&mut eval);

    return natural_frequencies_from_matrices(
        &eval.get_mass_matrix(),
        &eval.get_tangent_damping_matrix(),
        &eval.get_tangent_stiffness_matrix()
    );
}

pub fn natural_frequencies_from_matrices(M: &DVector<f64>, D: &DMatrix<f64>, K: &DMatrix<f64>) -> Vec<Mode> {
    let A = stack![
        0, K;
        K, D;
    ];

    let B = stack![
        K,  0;
        0, -DMatrix::from_diagonal(M)
    ];

    // Compute the complex eigenvalues for A*v = lambda*B*v
    let B_inv = B.try_inverse().expect("Failed to invert system matrix");
    let lambda = (B_inv*A).complex_eigenvalues();

    // Create natural frequency result for each pair of complex conjugated eigenvalues
    let mut results: Vec<Mode> = lambda.iter()
        .tuple_windows()
        .filter(|(l1, l2)| l1.conj().eq(l2))    // TODO: Should there be a small tolerance here?
        .map(|(l1, _)| Mode::new(*l1))
        .collect();

    // Sort results by undamped natural frequency
    results.sort_by(|a, b| a.omega0.partial_cmp(&b.omega0).expect("Failed to sort by frequency"));
    return results;
}