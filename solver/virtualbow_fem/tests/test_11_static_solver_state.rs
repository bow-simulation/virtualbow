#![allow(non_snake_case)]

use std::f64::consts::FRAC_PI_2;

use approx::assert_abs_diff_eq;
use assert_matches::assert_matches;
use iter_num_tools::lin_space;
use itertools::Itertools;

use virtualbow_fem::elements::string::StringElement;
use virtualbow_fem::solvers::statics::{DisplacementControl, LoadControl, StaticSolverError, StaticTolerances};
use virtualbow_fem::system::dof::DofType;
use virtualbow_fem::system::node::Node;
use virtualbow_fem::system::system::System;
use virtualbow_num::newton::NewtonSettings;

// Tests how the static solvers treat the state of the system when a solution fails.
//
// Background: The solvers apply trial displacements to the system on every Newton iteration.
// When the iterations fail, the system used to be left in the diverged state of the last
// iteration, which is not a solution and can be arbitrarily far off. Callers that evaluated
// results afterwards silently got meaningless values, which is how the bracing algorithm of
// the bow simulation ended up with a string slope of the wrong sign and magnitude.

const TOLERANCES: StaticTolerances = StaticTolerances { linear_pos: 1e-6, angular_pos: 1e-6, loadfactor: 1e-6 };

// Iteration limit that is sufficient for the system below
const SETTINGS_WORKING: NewtonSettings = NewtonSettings { max_iterations: 100, line_searching: None };

// Iteration limits that are insufficient for the respective load cases below. They have to be
// chosen such that the solver actually applies trial states to the system before it gives up.
// If it bails out on the very first iteration, the system stays untouched by accident and the
// tests would be vacuous, which can be checked by disabling the state restoration.
const SETTINGS_FAILING_DISPLACEMENT: NewtonSettings = NewtonSettings { max_iterations: 2, line_searching: None };
const SETTINGS_FAILING_LOAD: NewtonSettings = NewtonSettings { max_iterations: 1, line_searching: None };

const R: f64 = 1.5;

// Everything an outside observer can learn about the state of the system,
// including the internal state of the string element.
#[derive(Clone, Debug, PartialEq)]
struct Snapshot {
    u: Vec<f64>,
    v: Vec<f64>,
    length: f64,
    contacts: Vec<[f64; 2]>,
}

fn snapshot(system: &mut System, element: usize) -> Snapshot {
    let eval = system.evaluated();
    let string = eval.element::<StringElement>(element);

    Snapshot {
        u: eval.get_displacements().iter().copied().collect(),
        v: eval.get_velocities().iter().copied().collect(),
        length: string.get_current_length(),
        contacts: string.contact_positions().map(|p| [p[0], p[1]]).collect(),
    }
}

// Creates a string in contact with a fixed quarter-arc, with a free end node that can be
// displaced or loaded. Same setup as in test_03_static_string, chosen here because the string
// element carries internal state (contact points, current length) that has to be kept
// consistent with the state of the system.
fn create_system() -> (System, Node, usize) {
    let n = 100;
    let l0 = 0.5*R;
    let EA = 2100.0;

    let mut system = System::new();

    let fixed_nodes = lin_space(0.0..=FRAC_PI_2, n).map(|alpha| {
        system.create_node([R*f64::cos(alpha), R*f64::sin(alpha), 0.0], [DofType::Locked; 3])
    }).collect_vec();

    let free_node = system.create_node([R, R, 0.0], [DofType::Locked, DofType::Active, DofType::Locked]);

    // Reference load, needed by the displacement controlled solver to determine the load factor
    system.add_force(free_node.y(), |_t| { 1.0 });

    let mut string_nodes = vec![free_node];
    string_nodes.extend_from_slice(&fixed_nodes);

    let offsets = vec![0.0; string_nodes.len()];
    let element = system.add_element(&string_nodes, StringElement::new(EA, 0.0, l0, 1.0, offsets));

    (system, free_node, element)
}

// Moves the system away from its initial state, so that the tests below can tell a restoration
// of the previous state apart from a reset to the initial one.
fn apply_reference_solution(system: &mut System, free_node: Node) {
    let solver = DisplacementControl::new(system, TOLERANCES, SETTINGS_WORKING);
    solver.solve_equilibrium(free_node.y(), -0.1).expect("Reference solution failed");
}

#[test]
fn displacement_control_failure_leaves_system_unchanged() {
    let (mut system, free_node, element) = create_system();
    apply_reference_solution(&mut system, free_node);

    let before = snapshot(&mut system, element);

    // Pull the free node all the way down in a single step, which the solver can't manage
    let solver = DisplacementControl::new(&mut system, TOLERANCES, SETTINGS_FAILING_DISPLACEMENT);
    let result = solver.solve_equilibrium(free_node.y(), -R);

    assert_matches!(result, Err(StaticSolverError::EquilibriumError(_)));
    assert_eq!(snapshot(&mut system, element), before, "Failed solution must leave the system unchanged");
}

#[test]
fn load_control_failure_leaves_system_unchanged() {
    let (mut system, free_node, element) = create_system();
    apply_reference_solution(&mut system, free_node);

    let before = snapshot(&mut system, element);

    // Apply a load that is far too large to be reached with the given iteration limit
    system.add_force(free_node.y(), |_t| { -1.0e6 });

    let solver = LoadControl::new(&mut system, TOLERANCES, SETTINGS_FAILING_LOAD);
    let result = solver.solve_equilibrium();

    assert_matches!(result, Err(StaticSolverError::EquilibriumError(_)));
    assert_eq!(snapshot(&mut system, element), before, "Failed solution must leave the system unchanged");
}

#[test]
fn successful_solution_updates_system() {
    // Counterpart to the tests above: make sure the state is only restored on failure
    // and a successful solution still has its effect on the system.

    let (mut system, free_node, element) = create_system();
    let before = snapshot(&mut system, element);

    apply_reference_solution(&mut system, free_node);

    assert_ne!(snapshot(&mut system, element), before);
    assert_abs_diff_eq!(system.get_dof_displacement(free_node.y()), -0.1, epsilon=1e-9);
}

#[test]
fn abort_by_caller_keeps_last_equilibrium() {
    // An abort by the caller is not a numerical failure: the system is left at the last
    // equilibrium state that was actually reached, which is a valid solution.

    let (mut system, free_node, element) = create_system();
    let before = snapshot(&mut system, element);

    let mut steps = 0;
    let solver = DisplacementControl::new(&mut system, TOLERANCES, SETTINGS_WORKING);
    let result = solver.solve_equilibrium_path(free_node.y(), -R, 100, &mut |_system, _eval, _info| {
        steps += 1;
        steps < 10
    });

    assert_matches!(result, Err(StaticSolverError::AbortedByCaller));
    assert_ne!(snapshot(&mut system, element), before);

    // The solution stopped part way along the path, after 10 of 101 points
    assert_abs_diff_eq!(system.get_dof_displacement(free_node.y()), -9.0/100.0*R, epsilon=1e-9);
}
