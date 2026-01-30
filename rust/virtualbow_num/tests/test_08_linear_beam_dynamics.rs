use std::f64::consts::{FRAC_PI_2, TAU};
use approx::assert_abs_diff_eq;
use nalgebra::{SVector, vector};
use virtualbow_num::fem::elements::beam::beam::BeamElement;
use virtualbow_num::fem::elements::beam::geometry::CrossSection;
use virtualbow_num::fem::elements::beam::linear::LinearBeamSegment;
use virtualbow_num::fem::solvers::dynamics::{DynamicSolver, DynamicSolverSettings, DynamicTolerances, StopCondition, TimeStepping};
use virtualbow_num::fem::system::dof::DofType;
use virtualbow_num::fem::system::system::System;
use virtualbow_num::testutils::curves::Line;
use virtualbow_num::utils::integration::fixed_simpson;
use virtualbow_num::testutils::plotter::Plotter;
use virtualbow_num::testutils::sections::Section;

// This tests compares the analytical solution for the linear vibration of a straight cantilever beam with the numerical FEM solution.
// Unfortunately the analytical solution has its limitations too since numerical accuracy starts to become a problem at ~10 modes.
// This number of modes is equivalent to 20 degrees of freedom for the analytical model. The FEM model has (and needs) more degrees of freedom,
// which also probably causes the FEM solution to contain more of the smaller "wiggles".
// It is therefore difficult get the two solutions to match very closely in a numerical sense but looking at the plots shows that they are
// actually quite similar.
//
// Possible future work: Use arbitrary precision calculations for the analytical solution to get a "true" reference solution.

#[test]
fn test_linear_beam_dynamics() {
    // Beam parameters
    let l = 0.90;
    let w = 0.01;
    let h = 0.01;
    let r = 15.0*l;

    let ρ = 500.0;
    let E = 20e9;
    let G = 100e9;

    const N_ELEMENTS: usize = 25;
    const N_MODES: usize = 10;

    // Initial deflection: Circular arc
    let w0 = |x| r - f64::sqrt(r*r - x*x);
    let φ0 = |x| x/f64::sqrt(r*r - x*x);
    let v0 = |_| 0.0;

    // Initial deflection: Cubic polynomial (static solution)
    //let w0 = |x: f64| 0.01*x.powi(3)*(3.0*l - x);
    //let φ0 = |x: f64| 0.01*x.powi(2)*(9.0*l - 4.0*x);
    //let v1 = |_: f64| 0.0;

    let curve = Line::new(l);
    let section = Section::new(ρ, E, G, &[w], &[h], &[0.0]);

    let mut system = System::new();
    let mut nodes = Vec::new();

    // Create linear beam segments and elements
    let (segments, _points, x_nodes) = LinearBeamSegment::discretize(&curve, &section, N_ELEMENTS, 2);

    // Create nodes with initial positions
    for &x in &x_nodes {
        let kind = DofType::active_if(x != 0.0);
        let node = system.create_node(&vector![x, w0(x), φ0(x)], &[kind; 3]);
        nodes.push(node);
    }

    // Create beam elements
    segments.iter().enumerate().for_each(|(i, segment)| {
        system.add_element(&[nodes[i], nodes[i+1]], BeamElement::new(segment));
    });

    let beam = ContinuousBeam::<N_MODES>::new(section.mass(0.0)[(0, 0)], section.stiffness(0.0)[(2, 2)], l, w0, v0);
    let period = TAU/beam.ω[0];  // Period of the first natural frequency
    let t_end = 0.25*period;

    let tolerances = DynamicTolerances {
        linear_acc: 1e-4,
        angular_acc: 1e-3,
        loadfactor: 1e-3,
    };

    let settings = DynamicSolverSettings {
        time_stepping: TimeStepping::Adaptive {
            min_timestep: 1e-6,
            max_timestep: 1e-3,
            steps_per_period: 500
        },
        ..Default::default()
    };

    let mut plotter = Plotter::new();
    let mut solver = DynamicSolver::new(&mut system, tolerances, settings);
    solver.solve(StopCondition::Time(t_end), &mut |system, _eval| {
        for i in 0..nodes.len() {
            let t = system.get_time();
            let x = x_nodes[i];

            let w_num = system.get_position(nodes[i].y());
            let φ_num = system.get_position(nodes[i].φ());

            let w_ref = beam.w(x, t);
            let φ_ref = beam.φ(x, t);

            plotter.add_point((t, w_num), (t, w_ref), &format!("Deflection at x={x:.3}"), "Time [s]", "Position [m]");
            plotter.add_point((t, φ_num), (t, φ_ref), &format!("Angle at x={x:.3}"), "Time [s]", "Angle [m]");

            assert_abs_diff_eq!(w_num, w_ref, epsilon=1e-2*w0(l));
            assert_abs_diff_eq!(φ_num, φ_num, epsilon=1e-2*φ0(l));
        }

        return true;
    }).unwrap();

    for node in &nodes {
        let x = system.get_position(node.x());
        let y = system.get_position(node.y());
        let φ = system.get_position(node.φ());

        plotter.add_point((x, y), (x, beam.w(x, system.get_time())), "Final Deflection", "x [m]", "y [m]");
        plotter.add_point((x, φ), (x, beam.φ(x, system.get_time())), "Final Angle", "x [m]", "φ [rad]");
    }
}

// Utility for computing the analytical solution for the linear dynamics of a cantilever beam
// with an accuracy of N modes
struct ContinuousBeam<const N: usize> {
    l: f64,                 // Length of the beam
    k: SVector<f64, N>,    // Shape function constants for each mode
    ω: SVector<f64, N>,    // Natural frequencies for each mode
    A: SVector<f64, N>,    // Initial conditions for each mode (1)
    B: SVector<f64, N>,    // Initial conditions for each mode (2)
}

impl<const N: usize> ContinuousBeam<N> {
    fn new<Fw, Fv>(ρA: f64, EI: f64, l: f64, w0: Fw, v0: Fv) -> Self
    where Fw: Fn(f64) -> f64, Fv: Fn(f64) -> f64
    {
        let kappa = |i| {
            1.0/l * match i {
                1 => 1.87510406871196,
                2 => 4.69409113297417,
                3 => 7.85475743823761,
                4 => 10.9955407348755,
                _ => ((2*i - 1) as f64)*FRAC_PI_2
            }
        };

        let k: SVector<f64, N> = SVector::from_fn(|i, _| kappa(i+1));
        let ω: SVector<f64, N> = k.component_mul(&k)*f64::sqrt(EI/ρA);

        let n_int = 250*(f64::ceil(k[N-1]/TAU) as usize);    // Integration points proportional to the number of periods of the last natural shape

        let Iw = fixed_simpson(|x| w0(x)*Self::W(&k, l, x), 0.0, l, n_int);
        let Iv = fixed_simpson(|x| v0(x)*Self::W(&k, l, x), 0.0, l, n_int);

        let A = Iw/l;
        let B = Iv.component_div(&(ω*l));

        Self {
            l,
            k,
            ω,
            A,
            B
        }
    }

    // Evaluates the shape function for all modes
    fn W(k: &SVector<f64, N>, l: f64, x: f64) -> SVector<f64, N> {
        let gamma = ((k*l).map(f64::cos) + (k*l).map(f64::cosh)).component_div(&((k*l).map(f64::sin) + (k*l).map(f64::sinh)));
        (k*x).map(f64::cos) - (k*x).map(f64::cosh) - gamma.component_mul(&((k*x).map(f64::sin) - (k*x).map(f64::sinh)))
    }

    // Evaluates the derivative of the shape function for all modes
    fn dWdx(k: &SVector<f64, N>, l: f64, x: f64) -> SVector<f64, N> {
        let gamma = ((k*l).map(f64::cos) + (k*l).map(f64::cosh)).component_div(&((k*l).map(f64::sin) + (k*l).map(f64::sinh)));
        -k.component_mul(&(
            (k*x).map(f64::sin) + (k*x).map(f64::sinh) + gamma.component_mul(&((k*x).map(f64::cos) - (k*x).map(f64::cosh)))
        ))
    }

    // Evaluates the deflection at position x and time t
    fn w(&self, x: f64, t: f64) -> f64 {
        let H = self.A.component_mul(&(self.ω*t).map(f64::cos)) + self.B.component_mul(&(self.ω*t).map(f64::sin));
        let W = Self::W(&self.k, self.l, x);
        W.component_mul(&H).sum()
    }

    // Evaluates the deflection angle at position x and time t
    fn φ(&self, x: f64, t: f64) -> f64 {
        let H = self.A.component_mul(&(self.ω*t).map(f64::cos)) + self.B.component_mul(&(self.ω*t).map(f64::sin));
        let dWdx = Self::dWdx(&self.k, self.l, x);
        dWdx.component_mul(&H).sum()
    }
}