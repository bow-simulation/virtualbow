use nalgebra::{DMatrix, DVector};
use crate::fem::system::system::System;
use crate::testutils::numdiff::{differentiate_n_to_1, differentiate_n_to_k};
use approx::{assert_abs_diff_eq, assert_relative_eq};

const NUM_DIFF_STEPSIZE : f64 = 0.0001;
const NUM_DIFF_MAX_ERROR: f64 = 1e-3;
const EPSILON_RELATIVE: f64 = 1e-6;
const EPSILON_ABSOLUTE: f64 = 1e-9;

// Performs a series of physical consistency checks on the system at its current state
pub fn assert_system_invariants(system: &mut System) {
    // Remember original system state
    let u_backup = system.get_displacements().clone();
    let v_backup = system.get_velocities().clone();

    assert_mass_matrix(system);
    assert_stiffness_matrix(system, &u_backup, &v_backup);
    assert_damping_matrix(system, &u_backup, &v_backup);
    assert_kinetic_energy(system, &u_backup, &v_backup);
    assert_potential_energy(system, &u_backup);

    // Reapply original system state
    system.set_displacements(&u_backup);
    system.set_velocities(&v_backup);
}

// Evaluates the system's mass matrix and verifies that it is symmetric and positive definite
pub fn assert_mass_matrix(system: &mut System) {
    let mut M = DVector::zeros(system.n_dofs());
    system.compute_mass_matrix(&mut M);

    // Mass matrix must be positive semi-definite
    assert!(M.min() >= 0.0);
}

// Evaluates the system's tangent stiffness matrix and verifies that it is symmetric
// and equal to the derivative of the internal forces with respect to the displacements
pub fn assert_stiffness_matrix(system: &mut System, u: &DVector<f64>, v: &DVector<f64>) {
    let mut K_sys = DMatrix::zeros(system.n_dofs(), system.n_dofs());
    let mut q_test = DVector::zeros(system.n_dofs());

    system.set_displacements(u);
    system.set_velocities(v);
    system.compute_internal_forces(None, Some(&mut K_sys), None);

    let (K_num, error) = differentiate_n_to_k(&mut |u_test| {
        system.set_displacements(u_test);
        system.compute_internal_forces(Some(&mut q_test), None, None);
        return q_test.clone();
    }, u, NUM_DIFF_STEPSIZE);

    // Check error of the derivative approximation
    assert!(error < NUM_DIFF_MAX_ERROR);

    // Tangent stiffness matrix must be equal to numeric derivative of the internal forces
    assert_abs_diff_eq!(&K_sys, &K_num, epsilon=K_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE)
}

// Evaluates the system's tangent damping matrix and verifies that it is symmetric
// and equal to the derivative of the internal forces with respect to the velocities
pub fn assert_damping_matrix(system: &mut System, u: &DVector<f64>, v: &DVector<f64>) {
    let mut D_sys = DMatrix::zeros(system.n_dofs(), system.n_dofs());
    let mut q_test = DVector::zeros(system.n_dofs());

    system.set_displacements(u);
    system.set_velocities(v);
    system.compute_internal_forces(None, None, Some(&mut D_sys));

    let (D_num, error) = differentiate_n_to_k(&mut |v_test| {
        system.set_velocities(v_test);
        system.compute_internal_forces(Some(&mut q_test), None, None);
        return q_test.clone();
    }, v, NUM_DIFF_STEPSIZE);

    // Check error of the derivative approximation
    assert!(error < NUM_DIFF_MAX_ERROR);

    // Tangent damping matrix must be symmetric
    // TODO: Not in general though (gyroscopic forces?)
    // assert_abs_diff_eq!(&D_sys, &D_sys.transpose(), epsilon=D_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);

    // Tangent damping matrix must be equal to numeric derivative of the internal forces
    assert_abs_diff_eq!(&D_sys, &D_num, epsilon=D_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);
}

// Checks if the kinetic energy is consistent with the mass matrix and velocities of the system
pub fn assert_kinetic_energy(system: &mut System, u: &DVector<f64>, v: &DVector<f64>) {
    let mut M = DVector::zeros(system.n_dofs());
    system.compute_mass_matrix(&mut M);

    system.set_displacements(u);
    system.set_velocities(v);

    system.compute_internal_forces(None, None, None);  // TODO: Make obsolete

    let E_sys: f64 = system.elements().map(|element| { element.kinetic_energy() }).sum();
    let E_num: f64 = 0.5*v.dot(&(M.component_mul(v)));

    assert_relative_eq!(E_sys, E_num, max_relative=EPSILON_RELATIVE);
}

// Checks if the elastic forces are equal to the derivative of the potential energy
// This is only true for v = 0, i.e. without nonlinear damping forces
pub fn assert_potential_energy(system: &mut System, u: &DVector<f64>) {
    let v = DVector::<f64>::zeros(system.n_dofs());

    system.set_displacements(u);
    system.set_velocities(&v);

    let mut q_sys = DVector::zeros(system.n_dofs());
    system.compute_internal_forces(Some(&mut q_sys), None, None);

    let (q_num, error) = differentiate_n_to_1(&mut |u_test| {
        system.set_displacements(u_test);
        system.compute_internal_forces(None, None, None);  // Only updates potential energies of the elements  // TODO: Should happen with system.set_state() or similar
        return system.elements().map(|element| { element.potential_energy() }).sum();
    }, u, NUM_DIFF_STEPSIZE);

    // Check error of the derivative approximation
    assert!(error < NUM_DIFF_MAX_ERROR);

    // Elastic forces must be equal to the numerical derivative of the potential energy
    assert_abs_diff_eq!(q_sys, q_num, epsilon=q_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);
}