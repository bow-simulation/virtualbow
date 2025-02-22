use std::f64::consts::{FRAC_PI_2, TAU};
use approx::assert_abs_diff_eq;
use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::{SVector, vector};
use virtualbow::input::{Height, Layer, Line, Material, ProfileAlignment, Width};
use virtualbow::profile::profile::CurvePoint;
use virtualbow::profile::segments::clothoid::ClothoidSegment;
use virtualbow::sections::section::LayeredCrossSection;
use virtualbow_num::fem::elements::beam::beam::BeamElement;
use virtualbow_num::fem::elements::beam::geometry::CrossSection;
use virtualbow_num::fem::elements::beam::linear::LinearBeamSegment;
use virtualbow_num::fem::solvers::dynamics::{DynamicSolver, DynamicSolverSettings, StopCondition, TimeStepping};
use virtualbow_num::fem::system::system::System;
use virtualbow_num::utils::integration::fixed_simpson;
use virtualbow_num::utils::plotter::Plotter;

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

    let E = 20e9;
    let G = 100e9;
    let ρ = 500.0;

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

    let x_nodes = lin_space(0.0..=l, N_ELEMENTS+1).collect_vec();
    let start = CurvePoint::zero();
    let curve = ClothoidSegment::line(&start, &Line::new(l));

    let material = Material::new("material", "#000000", ρ, E, G);
    let width = Width::constant(w);
    let layer = Layer::new("layer", "material", Height::constant(h));
    let section = LayeredCrossSection::new(&width, &vec![layer], &vec![material], &ProfileAlignment::SectionCenter).unwrap();

    let mut system = System::new();
    let mut nodes = Vec::new();

    // Create nodes with initial positions
    for &x in &x_nodes {
        let free = x != 0.0;
        let node = system.create_node(&vector![x, w0(x), φ0(x)], &[free; 3]);
        nodes.push(node);
    }

    // Create beam elements
    for ((i0, &x0), (i1, &x1)) in x_nodes.iter().enumerate().tuple_windows() {
        let segment = LinearBeamSegment::new(&curve, &section, x0, x1, &[]);
        let element = BeamElement::new(&segment);
        system.add_element(&[nodes[i0], nodes[i1]], element);
    }

    let beam = ContinuousBeam::<N_MODES>::new(section.ρA(0.0), section.C(0.0)[(1, 1)], l, w0, v0);
    let period = TAU/beam.ω[0];  // Period of the first natural frequency
    let t_end = 0.25*period;

    let mut plotter = Plotter::new();

    let mut solver = DynamicSolver::new(&mut system, DynamicSolverSettings { time_stepping: TimeStepping::Adaptive { min_timestep: 1e-6, max_timestep: 1e-3, steps_per_period: 500 }, ..Default::default() });
    solver.solve(StopCondition::Time(t_end), &mut |system, _eval| {
        for i in 0..nodes.len() {
            let x = x_nodes[i];
            let t = system.get_time();

            let w_num = system.get_displacement(nodes[i].y());
            let φ_num = system.get_displacement(nodes[i].φ());

            let w_ref = beam.w(x, t);
            let φ_ref = beam.φ(x, t);

            plotter.add_point((t, w_num), (t, w_ref), &format!("Deflection at x={:.3}", x), "Time [s]", "Position [m]");
            plotter.add_point((t, φ_num), (t, φ_ref), &format!("Angle at x={:.3}", x), "Time [s]", "Angle [m]");

            assert_abs_diff_eq!(w_num, w_ref, epsilon=1e-2*w0(l));
            assert_abs_diff_eq!(φ_num, φ_num, epsilon=1e-2*φ0(l));
        }

        return true;
    }).unwrap();

    for node in &nodes {
        let x = system.get_displacement(node.x());
        let y = system.get_displacement(node.y());
        let φ = system.get_displacement(node.φ());

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