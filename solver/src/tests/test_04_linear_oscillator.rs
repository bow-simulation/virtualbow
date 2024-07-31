use crate::fem::solvers::eigen::natural_frequencies_from_matrices;
use std::f64::consts::{PI, TAU};
use iter_num_tools::lin_space;
use nalgebra::{Complex, ComplexField, DMatrix, DVector, Dyn, LU, stack};
use crate::fem::elements::bar::BarElement;
use crate::fem::elements::mass::MassElement;
use crate::fem::solvers::dynamics::{DynamicSolverRK4, Settings};
use crate::fem::system::nodes::{Constraints, PointNode};
use crate::fem::system::system::System;
use crate::tests::utils;
use crate::tests::utils::plotter::Plotter;

// These tests verify that the dynamics of linear mass-spring-damper systems is solved correctly.

#[test]
fn mass_spring_damper_1() {
    // Single degree of freedom oscillator without external forces, started from an initial position with zero initial velocity.
    // The system is simulated dynamically and position, velocity and acceleration is compared to an analytical solution.
    // https://de.wikipedia.org/wiki/Schwingung#Linear_ged.C3.A4mpfte_Schwingung

    let l = 1.0;
    let k = 100.0;
    let d = 10.0;
    let m = 5.0;

    let x0 = 0.1;   // Initial displacement

    let mut system = System::new();
    let node_a = system.create_xy_node(0.0, 0.0, Constraints::all_fixed());
    let node_b = system.create_xy_node(l + x0, 0.0, Constraints::x_pos_free());

    system.add_element(&[node_a, node_b], BarElement::spring(0.0, d, k, l));
    system.add_element(&[node_b], MassElement::new(m));
    utils::checks::check_system_invariants(&mut system);

    // Constants for the analytical solution
    let delta = d/(2.0*m);                                // Decay constant
    let omega0 = f64::sqrt(k/m);                          // Natural frequency of the undamped system
    let omega = f64::sqrt(omega0*omega0 - delta*delta);   // Natural frequency
    let T = 2.0*PI/omega;                                 // Period length
    let A = delta/omega*x0;
    let B = x0;

    assert!(delta < omega0);    // Make sure the system is underdamped

    let mut plotter = Plotter::new();
    let mut solver = DynamicSolverRK4::new(&mut system, Settings { timestep: 0.001 });

    solver.solve(&mut |system, dynamics| {
        // Numerical solution
        let x_sys = system.get_displacement(node_b.x()) - l;
        let v_sys = system.get_velocity(node_b.x());
        let a_sys = dynamics.get_acceleration(node_b.x());

        // Analytical solution
        let t = system.get_time();
        let x_ref = f64::exp(-delta*t)*(A*f64::sin(omega*t) + B*f64::cos(omega*t));
        let v_ref = -f64::exp(-delta*t)*((omega*B + delta*A)*f64::sin(omega*t) + (delta*B - omega*A)*f64::cos(omega*t));
        let a_ref = f64::exp(-delta*t)*(-B*(omega*omega + delta*delta)*f64::cos(omega*t) + (A*(delta*delta - omega*omega) + 2.0*B*omega*delta)*f64::sin(omega*t));

        plotter.add_point((system.get_time(), x_sys), (system.get_time(), x_ref), "position", "Time [s]", "Position [m]");
        plotter.add_point((system.get_time(), v_sys), (system.get_time(), v_ref), "velocity", "Time [s]", "Velocity [m/s]");
        plotter.add_point((system.get_time(), a_sys), (system.get_time(), a_ref), "acceleration", "Time [s]", "Acceleration [m/s]");

        assert!(f64::abs(x_sys - x_ref) < 6e-5);
        assert!(f64::abs(v_sys - v_ref) < 3e-4);
        assert!(f64::abs(a_sys - a_ref) < 2e-3);

        return system.get_time() < T;
    });
}


#[test]
fn mass_spring_damper_n() {
    // Multi degree of freedom mass-spring-damper system with a harmonic excitation force.
    // The system is simulated and compared to an analytical solution (see utility solver below) in terms of position, velocity and acceleration.

    // Parameters
    let m = 0.1;
    let k = 1e5;
    let d = 1.0;

    let n = 5;
    let L = 1.0;

    // Analytical system matrices

    let M = DVector::<f64>::from_element(n, m);

    let D = DMatrix::<f64>::from_fn(n, n, |i, j| {
        match i.abs_diff(j) {
            0 => 2.0*d,
            1 => -d,
            _ => 0.0
        }
    });

    let K = DMatrix::<f64>::from_fn(n, n, |i, j| {
        match i.abs_diff(j) {
            0 => 2.0*k,
            1 => -k,
            _ => 0.0
        }
    });

    // Random constant excitation and initial displacements

    let u_max = L/(10.0*(n as f64));
    let p_max = k*u_max;

    let p0 = p_max*DVector::<f64>::new_random(n);
    let u0 = u_max*DVector::<f64>::new_random(n);
    let v0 = DVector::<f64>::zeros(n);

    // Compute period of the first eigenmode
    let modes = natural_frequencies_from_matrices(&M, &D, &K);
    let period = TAU/modes[0].omega0;    // Period of the lowest eigenfrequency
    let omega = 5.0*modes[0].omega0;     // Frequency of the excitation

    // Reference solution
    let ref_solver = LinearSolver::new(&DMatrix::from_diagonal(&M), &D, &K, &u0, &v0, &p0, omega);

    // Finite element solution

    let mut system = System::new();
    let mut nodes = Vec::<PointNode>::new();

    let lengths: Vec<f64> = lin_space(0.0..=L, n+2).collect();
    for (i, s) in lengths.iter().enumerate() {
        let (position, constraints) = if (*s != 0.0) && (*s != L) {
            (*s + u0[i-1], Constraints::x_pos_free())
        } else {
            (*s, Constraints::all_fixed())
        };
        nodes.push(system.create_xy_node(position, 0.0, constraints));
    }

    // Add bar elements between nodes
    for i in 0..nodes.len()-1 {
        let l = lengths[i+1] - lengths[i];
        system.add_element(&[nodes[i], nodes[i+1]], BarElement::spring(0.0, d, k, l));
    }

    // Add mass elements at nodes
    for i in 1..nodes.len()-1 {
        system.add_element(&[nodes[i]], MassElement::new(m));
    }

    // Add external forces
    for i in 0..p0.len() {
        let p = p0[i];
        system.add_force(nodes[i+1].x(), move |t| p*f64::cos(omega*t));
    }

    utils::checks::check_system_invariants(&mut system);

    let mut plotter = Plotter::new();
    let mut solver = DynamicSolverRK4::new(&mut system, Settings { timestep: period/10000.0 });

    solver.solve(&mut |system, dynamics| {
        // Evaluate fem system and reference solution
        let u_sys = DVector::<f64>::from_fn(system.n_dofs(), |i, _| { system.get_displacement(nodes[i+1].x()) - lengths[i+1] });
        let v_sys = DVector::<f64>::from_fn(system.n_dofs(), |i, _| { system.get_velocity(nodes[i+1].x()) });
        let a_sys = DVector::<f64>::from_fn(system.n_dofs(), |i, _| { dynamics.get_acceleration(nodes[i+1].x()) });
        let (u_ref, v_ref, a_ref) = ref_solver.evaluate(system.get_time());

        for i in 0..n {
            plotter.add_point((system.get_time(), u_sys[i]), (system.get_time(), u_ref[i]), &format!("position_{:02}", i), "Time [s]", "Position [m]");
            plotter.add_point((system.get_time(), v_sys[i]), (system.get_time(), v_ref[i]), &format!("velocity_{:02}", i), "Time [s]", "Velocity [m/s]");
            plotter.add_point((system.get_time(), a_sys[i]), (system.get_time(), a_ref[i]), &format!("acceleration_{:02}", i), "Time [s]", "Acceleration [m/s]");
        }

        assert_abs_diff_eq!(u_sys, u_ref, epsilon=5e-3*u_max);                    // TODO
        assert_abs_diff_eq!(v_sys, v_ref, epsilon=5e-2*u_max/period);             // TODO
        //assert_abs_diff_eq!(a_sys, a_ref, epsilon=5e-2*u_max/(period*period));    // TODO

        return system.get_time() < period;
    });
}

// Analytical solver for linear dynamic systems with harmonic excitations.
// See theory manual for more details.
struct LinearSolver {
    A: DMatrix<f64>,
    M: DMatrix<f64>,
    D: DMatrix<f64>,
    K: DMatrix<f64>,
    I: DMatrix<f64>,
    C1: LU<Complex<f64>, Dyn, Dyn>,
    p0: DVector<f64>,
    b0: DVector<f64>,
    x0: DVector<f64>,
    omega: f64
}

impl LinearSolver {
    fn new(M: &DMatrix<f64>, D: &DMatrix<f64>, K: &DMatrix<f64>, u0: &DVector<f64>, v0: &DVector<f64>, p0: &DVector<f64>, omega: f64) -> Self {
        let n = M.nrows();
        let decomp_M = M.clone().lu();

        // Initial state
        let x0 = stack![
            u0;
            v0
        ];

        // System matrix
        let A = stack![
            DMatrix::<f64>::zeros(n, n), DMatrix::<f64>::identity(n, n);
            -decomp_M.solve(K).unwrap(), -decomp_M.solve(D).unwrap()
        ];

        // Excitation vector
        let b0 = stack![
            DVector::<f64>::zeros(n);
            decomp_M.solve(p0).unwrap()
        ];

        // Inverse of A times excitation
        let I = DMatrix::<f64>::identity(2*n, 2*n);

        let i = Complex::<f64>::i();
        let C1 = DMatrix::<Complex<f64>>::from_fn(2*n, 2*n, |row, col| {
            I[(row, col)] - 1.0/(i*omega)*A[(row, col)]
        });

        Self {
            A,
            M: M.clone(),
            D: D.clone(),
            K: K.clone(),
            I,
            C1: C1.lu(),
            p0: p0.clone(),
            b0,
            x0,
            omega
        }
    }

    // Computes displacements, velocities and accelerations (u, v, a) at the given time t
    fn evaluate(&self, t: f64) -> (DVector<f64>, DVector<f64>, DVector<f64>) {
        let Phi = (&self.A*t).exp();

        let n = self.A.nrows()/2;
        let i = Complex::<f64>::i();

        let C2 = DMatrix::<Complex<f64>>::from_fn(2*n, 2*n, |row, col| {
            1.0/(i*self.omega)*(self.I[(row, col)]*(i*self.omega*t).exp() - Phi[(row, col)])
        });

        let C_im = self.C1.solve(&C2).unwrap();
        let C_re = DMatrix::<f64>::from_fn(2*n, 2*n, |row, col| { C_im[(row, col)].real() });

        let x = &Phi*&self.x0 + &C_re*&self.b0;
        let u = x.rows(0, n).into();
        let v = x.rows(n, n).into();

        // Compute acceleration
        let decomp_M = self.M.clone().lu();
        let p: DVector<f64> = &self.p0*f64::cos(self.omega*t);
        let q: DVector<f64> = &self.K*&u - &self.D*&v;
        let a: DVector<f64> = decomp_M.solve(&(p - q)).unwrap();

        return (u, v, a);
    }
}
