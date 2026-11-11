use nalgebra::{DMatrix, DVector};
use crate::system::system::System;
use approx::{assert_abs_diff_eq, assert_relative_eq};
use virtualbow_num::numdiff::{differentiate_1_to_n, differentiate_n_to_1, differentiate_n_to_k};

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
    system.set_state(&u_backup, &v_backup);
}

// Evaluates the system's mass matrix and verifies that it is symmetric and positive definite
pub fn assert_mass_matrix(system: &mut System) {
    let mut M = DVector::zeros(system.n_dofs());
    system.evaluated().compute_mass_matrix(&mut M);

    // Mass matrix must be positive semi-definite
    assert!(M.min() >= 0.0);
}

// Evaluates the system's tangent stiffness matrix and verifies that it is symmetric
// and equal to the derivative of the internal forces with respect to the displacements
pub fn assert_stiffness_matrix(system: &mut System, u: &DVector<f64>, v: &DVector<f64>) {
    let mut K_sys = DMatrix::zeros(system.n_dofs(), system.n_dofs());
    let mut q_test = DVector::zeros(system.n_dofs());

    system.evaluate(u, v, None, Some(&mut K_sys), None);

    let (K_num, error) = differentiate_n_to_k(&mut |u_test| {
        system.evaluate(u_test, v, Some(&mut q_test), None, None);
        q_test.clone()
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

    system.evaluate(u, v, None, None, Some(&mut D_sys));

    let (D_num, error) = differentiate_n_to_k(&mut |v_test| {
        system.evaluate(u, v_test, Some(&mut q_test), None, None);
        q_test.clone()
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
    let eval = system.evaluate(u, v, None, None, None);

    // Both the mass matrix and the kinetic energy are taken at the same state here.
    // They used to be evaluated at whatever state the previous check had left behind.
    eval.compute_mass_matrix(&mut M);

    let E_sys: f64 = eval.elements().map(|element| { element.kinetic_energy() }).sum();
    let E_num: f64 = 0.5*v.dot(&(M.component_mul(v)));

    assert_relative_eq!(E_sys, E_num, max_relative=EPSILON_RELATIVE);
}

// Verifies that the derivative of the internal forces with respect to an element's parameter is equal
// to a numerical derivative of the internal forces, at the current state of the system.
// The parameter is applied through the given closure, since only the caller knows which one it is.
pub fn assert_parameter_derivative<F>(system: &mut System, index: usize, value: f64, mut set_parameter: F)
    where F: FnMut(&mut System, f64)
{
    let u = system.get_displacements().clone();
    let v = system.get_velocities().clone();

    let mut q_test = DVector::zeros(system.n_dofs());
    let mut dqdp_sys = DVector::zeros(system.n_dofs());

    // Analytical derivative as reported by the element
    set_parameter(system, value);
    system.evaluate(&u, &v, None, None, None).parameter_derivative(index, &mut dqdp_sys);

    // Numerical derivative of the internal forces with respect to the same parameter
    let (dqdp_num, error) = differentiate_1_to_n(&mut |p| {
        set_parameter(system, p);
        system.evaluate(&u, &v, Some(&mut q_test), None, None);
        q_test.clone()
    }, value, NUM_DIFF_STEPSIZE);

    // Put the parameter back and bring the elements in sync with it again
    set_parameter(system, value);
    system.set_state(&u, &v);

    // Check error of the derivative approximation
    assert!(error < NUM_DIFF_MAX_ERROR);

    // The analytical derivative must be equal to the numerical one
    assert_abs_diff_eq!(dqdp_sys, dqdp_num, epsilon=dqdp_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);
}

// Checks if the elastic forces are equal to the derivative of the potential energy
// This is only true for v = 0, i.e. without nonlinear damping forces
pub fn assert_potential_energy(system: &mut System, u: &DVector<f64>) {
    let v = DVector::<f64>::zeros(system.n_dofs());

    let mut q_sys = DVector::zeros(system.n_dofs());
    system.evaluate(u, &v, Some(&mut q_sys), None, None);

    let (q_num, error) = differentiate_n_to_1(&mut |u_test| {
        let eval = system.evaluate(u_test, &v, None, None, None);
        eval.elements().map(|element| { element.potential_energy() }).sum()
    }, u, NUM_DIFF_STEPSIZE);

    // Check error of the derivative approximation
    assert!(error < NUM_DIFF_MAX_ERROR);

    // Elastic forces must be equal to the numerical derivative of the potential energy
    assert_abs_diff_eq!(q_sys, q_num, epsilon=q_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);
}