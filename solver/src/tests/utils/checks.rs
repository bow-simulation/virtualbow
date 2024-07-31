use nalgebra::DVector;
use crate::fem::system::system::System;
use crate::numerics::differentiation::{differentiate_n_to_1, differentiate_n_to_k};

const NUM_DIFF_STEPSIZE : f64 = 0.0001;
const NUM_DIFF_MAX_ERROR: f64 = 1e-3;
const EPSILON_RELATIVE: f64 = 1e-6;
const EPSILON_ABSOLUTE: f64 = 1e-9;

// Performs a series of physical consistency checks on the system
// for varying, random system states (displacements and velocities)
pub fn check_system_invariants(system: &mut System) {
    // Remember original system state
    let u_backup = system.get_displacements().clone();
    let v_backup = system.get_velocities().clone();

    // Generate random states and check each of them
    let n_samples = 10*system.n_dofs();
    for _ in 0..n_samples {
        let u = &u_backup + 0.1*DVector::<f64>::new_random(system.n_dofs()).add_scalar(-0.5);
        let v = &v_backup + 0.1*DVector::<f64>::new_random(system.n_dofs()).add_scalar(-0.5);

        check_mass_matrix(system);
        check_stiffness_matrix(system, &u, &v);
        check_damping_matrix(system, &u, &v);

        check_kinetic_energy(system, &u, &v);
        check_potential_energy(system, &u);
    }

    // Reapply original system state
    system.set_displacements(&u_backup);
    system.set_velocities(&v_backup);
}

// Evaluates the system's mass matrix and verifies that it is symmetric and positive definite
pub fn check_mass_matrix(system: &mut System) {
    let mut eigen = system.default_eigen_eval();
    system.eval_eigen(&mut eigen);

    // Mass matrix must be positive definite
    assert!(eigen.get_mass_matrix().min() > 0.0);
}

// Evaluates the system's tangent stiffness matrix and verifies that it is symmetric
// and equal to the derivative of the internal forces with respect to the displacements
pub fn check_stiffness_matrix(system: &mut System, u: &DVector<f64>, v: &DVector<f64>) {
    let mut statics = system.default_static_eval();

    system.set_displacements(&u);
    system.set_velocities(&v);
    system.eval_statics(&mut statics);

    let K_sys = statics.get_tangent_stiffness_matrix().clone();

    let (K_num, _error) = differentiate_n_to_k(&mut |u_test| {
        system.set_displacements(&u_test);
        system.eval_statics(&mut statics);
        return statics.get_internal_forces().clone();
    }, &u, NUM_DIFF_STEPSIZE);

    // Check error of the derivative approximation
    // assert!(error < NUM_DIFF_MAX_ERROR);

    // Tangent stiffness matrix must be symmetric
    // TODO: Not under the influence of damping though, test this with v = 0
    // assert_abs_diff_eq!(&K_sys, &K_sys.transpose(), epsilon=K_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);

    // Tangent stiffness matrix must be equal to numeric derivative
    assert_abs_diff_eq!(&K_sys, &K_num, epsilon=K_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE)
}

// Evaluates the system's tangent damping matrix and verifies that it is symmetric
// and equal to the derivative of the internal forces with respect to the velocities
pub fn check_damping_matrix(system: &mut System, u: &DVector<f64>, v: &DVector<f64>) {
    // TODO: Replace bothj benlow by single evaluation of implicit dynamics
    let mut eigen = system.default_eigen_eval();
    let mut statics = system.default_static_eval();

    system.set_displacements(&u);
    system.set_velocities(&v);
    system.eval_eigen(&mut eigen);

    let D_sys = eigen.get_tangent_damping_matrix().clone();

    let (D_num, _error) = differentiate_n_to_k(&mut |v_test| {
        system.set_velocities(&v_test);
        system.eval_statics(&mut statics);
        return statics.get_internal_forces().clone();
    }, &u, NUM_DIFF_STEPSIZE);

    // Check error of the derivative approximation
    // assert!(error < NUM_DIFF_MAX_ERROR);

    // Tangent damping matrix must be symmetric
    // TODO: Not in general though (gyroscopic forces?)
    // assert_abs_diff_eq!(&D_sys, &D_sys.transpose(), epsilon=D_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);

    // Tangent damping matrix must be equal to numeric derivative
    assert_abs_diff_eq!(&D_sys, &D_num, epsilon=D_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);
}

// Checks if the kinetic energy is consistent with the mass matrix and velocities of the system
pub fn check_kinetic_energy(system: &mut System, u: &DVector<f64>, v: &DVector<f64>) {
    let mut eigen = system.default_eigen_eval();

    system.set_displacements(&u);
    system.set_velocities(&v);
    system.eval_eigen(&mut eigen);

    let E_sys: f64 = system.elements().map(|element| { element.kinetic_energy() }).sum();
    let E_num: f64 = 0.5*v.dot(&(eigen.get_mass_matrix().component_mul(v)));

    assert_relative_eq!(E_sys, E_num, max_relative=EPSILON_RELATIVE);
}

// Checks if the elastic forces are equal to the derivative of the potential energy
// This is only true for v = 0, i.e. without nonlinear damping forces
pub fn check_potential_energy(system: &mut System, u: &DVector<f64>) {
    let v = DVector::<f64>::zeros(system.n_dofs());

    let mut statics = system.default_static_eval();

    system.set_displacements(&u);
    system.set_velocities(&v);
    system.eval_statics(&mut statics);

    let q_sys = statics.get_internal_forces().clone();

    let (q_num, _error) = differentiate_n_to_1(&mut |u_test| {
        system.set_displacements(&u_test);
        system.eval_statics(&mut statics);
        return system.elements().map(|element| { element.potential_energy() }).sum();
    }, &u, NUM_DIFF_STEPSIZE);

    // Check error of the derivative approximation
    // assert!(error < NUM_DIFF_MAX_ERROR);

    // Elastic forces must be equal to the numerical derivative of the potential energy
    assert_abs_diff_eq!(q_sys, q_num, epsilon=q_num.amax()*EPSILON_RELATIVE + EPSILON_ABSOLUTE);
}