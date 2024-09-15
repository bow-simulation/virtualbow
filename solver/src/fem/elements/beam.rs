// Calculates the analytical stiffness matrix of a curved beam with varying cross section properties
// by using Castigliano's theorem and numerical integration. Method inspired by [1].
//
// [1] E. Marotta, P.Salvini: Analytical Stiffness Matrix for Curved Metal Wires,
// Procedia Structural Integrity, Volume 8, 2018, Pages 43-55
//
// r: s -> [x, y, phi], Function describing the shape of the beam segment with arc length s in [l0, l1]
// C: s -> [[Cee, Cek], [Cek, Ckk]], Function describing the material properties along the beam

use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::{matrix, vector, SMatrix, SVector, stack};
use crate::fem::system::element::Element;
use crate::fem::system::views::{MatrixView, PositionView, VectorView, VelocityView};
use crate::numerics::functions::normalize_angle;
use crate::numerics::integration::{integrate_fixed};

// Planar curve, parameterized over its arc length s
// Used to describe the reference line of a beam
pub trait PlanarCurve {
    // Arc length at the start of the curve
    fn s_start(&self) -> f64;

    // Arc length at the end of the curve
    fn s_end(&self) -> f64;

    // Arc length of the curve from start to end
    fn length(&self) -> f64 {
        self.s_end() - self.s_start()
    }

    // Curvature, first derivative of the tangent angle
    fn curvature(&self, s: f64) -> f64;

    // Angle between curve tangent and the x axis
    fn angle(&self, s: f64) -> f64;

    // Position vector [x(s), y(s)]
    fn position(&self, s: f64) -> SVector<f64, 2>;
}

// Cross section properties of a beam, parameterized over its arc length s
pub trait CrossSection {
    // Linear density, i.e. mass per unit length
    fn rhoA(&self, s: f64) -> f64;

    // Rotary inertia per unit length
    fn rhoI(&self, s: f64) -> f64;

    // Full cross section stiffness matrix that describes the relation
    // (epilon, kappa, gamma) -> (normal force, bending moment, shear force)
    fn C(&self, s: f64) -> SMatrix<f64, 3, 3>;

    // Total width and height, irrespective of any layers
    fn width(&self, s: f64) -> f64;
    fn height(&self, s: f64) -> f64;

    // Stress evaluation constants
    // TODO: Better name
    //fn stress(&self, s: f64) -> DMatrix<f64>;

    // Variation of strain over the cross section height
    fn strain(&self, epsilon: f64, kappa: f64, y: f64) -> f64 {
        epsilon - kappa*y
    }
}

pub struct LinearBeamSegment {
    pub s0: f64,                        // Start length
    pub s1: f64,                        // End length
    pub se: Vec<f64>,                   // Eval lengths

    pub p0: SVector<f64, 3>,            // Starting point (x, y, φ)
    pub p1: SVector<f64, 3>,            // Ending point (x, y, φ)
    pub pe: Vec<SVector<f64, 3>>,       // Eval points (x, y, φ)

    pub u_eval: Vec<SMatrix<f64, 3, 6>>,
    pub C_inv: Vec<SMatrix<f64, 3, 3>>,

    pub K: SMatrix<f64, 6, 6>,              // Stiffness matrix
    pub M: SVector<f64, 6>                  // Lumped mass matrix
}

impl LinearBeamSegment {
    pub fn new<C, S>(curve: &C, section: &S, s0: f64, s1: f64, s_eval: &[f64]) -> Self
        where C: PlanarCurve,
              S: CrossSection
    {
        assert!(s1 > s0, "Starting length must be larger than ending length");
        //assert!(s_eval.iter().all(|&s| s >= s0 && s <= s1), "Evaluation points must lie within start and endpoint");  // TODO: Check with tolerance?

        // Segment starting point
        let r0 = curve.position(s0);    // TODO: Let curve return position and angle in one call
        let φ0 = curve.angle(s0);

        // Segment endpoint
        let r1 = curve.position(s1);    // TODO: Let curve return position and angle in one call
        let φ1 = curve.angle(s1);

        // Evaluation points
        let pe = s_eval.iter().map(|&s| {
            let r = curve.position(s);    // TODO: Let curve return position and angle in one call
            let φ = curve.angle(s);
            vector![
                r[0],
                r[1],
                φ
            ]
        }).collect_vec();

        let H = |s, sn| -> SMatrix<f64, 3, 3> {
            let r = curve.position(s);
            let re = curve.position(sn);
            let φ = curve.angle(s);

            matrix![
                f64::cos(φ), f64::sin(φ), 0.0;
                r[1] - re[1], re[0] - r[0], 1.0;
                -f64::sin(φ), f64::cos(φ), 0.0;
            ]
        };

        let B = |si, sj| -> SMatrix<f64, 3, 3> {
            let ri = curve.position(si);
            let rj = curve.position(sj);
            matrix![
                -1.0, 0.0, 0.0;
                0.0, -1.0, 0.0;
                rj[1] - ri[1], ri[0] - rj[0], -1.0;
            ]
        };

        let dIds = |s| -> SMatrix<f64, 3, 3> {
            let H0 = H(s, s0);
            let C_inv = section.C(s).try_inverse().expect("Failed to invert section stiffness matrix");
            return H0.transpose()*C_inv*H0;
        };

        // Evaluate integral between each point of interest (segment bounds + eval points)
        // - Segment start to first eval point, if present
        // - Between each pair of eval points, if present
        // - Last eval point to segment end, if present

        // TODO: Nicer way to do this?
        let mut s_integ = Vec::with_capacity(2 + s_eval.len());
        s_integ.push(s0);
        s_integ.extend_from_slice(s_eval);
        s_integ.push(s1);

        let mut I = vec![SMatrix::<f64, 3, 3>::zeros()];
        s_integ.iter().tuple_windows().for_each(|(&sa, &sb)| {
            let last = I.last().unwrap();
            //I.push(last + integrate_adaptive(dIds, sa, sb, 1e-9, 10).expect("Failed to compute segment stiffness matrix"));  // TODO: Magic numbers
            I.push(last + integrate_fixed(dIds, sa, sb, 1000).expect("Failed to compute segment stiffness matrix"));  // TODO: Magic numbers  // TODO: Better integration method
        });

        // Compute inverse stiffness matrices at nodes and eval points

        let K0n_inv = s_eval.iter().enumerate().map(|(i, &sn)| { -B(s0, sn).transpose()*(I.last().unwrap() - I[i+1]) }).collect_vec();
        let K00_inv = I.last().unwrap();

        let K1n_inv = s_eval.iter().enumerate().map(|(i, &sn)| { B(s0, sn).transpose()*I[i+1]*B(s0, s1) }).collect_vec();
        let K11_inv = B(s0, s1).transpose()*I.last().unwrap()*B(s0, s1);

        // Complete stiffness matrix

        let K00 = K00_inv.try_inverse().expect("Failed to invert partial stiffness matrix");
        let K11 = K11_inv.try_inverse().expect("Failed to invert partial stiffness matrix");
        let K10 = B(s1, s0)*K00;
        let K01 = K10.transpose();

        let K = stack![
            K00, K01;
            K10, K11
        ];

        // Evaluation matrices

        let u_eval = s_eval.iter().enumerate().map(|(i, _)| {
            stack![K0n_inv[i]*K00, K1n_inv[i]*K11]
        }).collect();

        let C_inv = s_eval.iter().map(|&s| {
            section.C(s).try_inverse().expect("Failed to invert section stiffness matrix")
        }).collect();

        // Lumped mass matrix

        let sm = 0.5*(s0 + s1);  // Segment midpoint
        //let m0 = integrate_adaptive(|s|{ vector![ section.rhoA(s) ] }, s0, sm, 1e-9, 10).expect("Failed to compute segment mass")[0];    // Mass of the first segment half
        //let m1 = integrate_adaptive(|s|{ vector![ section.rhoA(s) ] }, sm, s1, 1e-9, 10).expect("Failed to compute segment mass")[0];    // Mass of the second segment half
        let m0 = integrate_fixed(|s|{ vector![ section.rhoA(s) ] }, s0, sm, 1000).expect("Failed to compute segment mass")[0];    // Mass of the first segment half  // TODO: Better integration method
        let m1 = integrate_fixed(|s|{ vector![ section.rhoA(s) ] }, sm, s1, 1000).expect("Failed to compute segment mass")[0];    // Mass of the second segment half  // TODO: Better integration method
        let J0 = 0.5*(s1 - s0)*section.rhoI(s0);    // Lumped rotary inertia of the first node
        let J1 = 0.5*(s1 - s0)*section.rhoI(s1);    // Lumped rotary inertia of the second node

        let M = vector![
            m0,
            m0,
            J0,
            m1,
            m1,
            J1,
        ];

        /*
        // Alternative: integrating both mass and rotary inertia at the nodes only
        // TODO: Test against natural frequencies and decide
        let M = 0.5*(s1 - s0)*vector![
            section.rhoA(s0),
            section.rhoA(s0),
            section.rhoI(s0),
            section.rhoA(s1),
            section.rhoA(s1),
            section.rhoI(s1)
        ];
        */

        Self {
            s0,
            s1,
            se: s_eval.to_vec(),
            p0: vector![r0[0], r0[1], φ0],    // TODO: Let curve return position and angle in one call
            p1: vector![r1[0], r1[1], φ1],    // TODO: Let curve return position and angle in one call
            pe,
            u_eval,
            C_inv,
            K,
            M,
        }
    }
}

pub struct BeamElement {
    // Constant data
    M: SVector<f64, 6>,       // Element mass matrix
    K: SMatrix<f64, 3, 3>,    // Linear stiffness matrix

    s0: f64,                   // Arc length at left node
    s1: f64,                   // Arc length at right node
    se: Vec<f64>,              // Evaluation lengths
    pe: Vec<SVector<f64, 3>>,  // Initial positions wrt. local reference frame

    l0: f64,                  // Initial length along x axis
    β0: f64,                  // Angular offset at left node
    β1: f64,                  // Angular offset at right node

    u_eval: Vec<SMatrix<f64, 3, 3>>,    // Displacement evaluation
    C_inv: Vec<SMatrix<f64, 3, 3>>,     // Inverse cross section stiffness

    // State dependent data

    u: SVector<f64, 3>,       // Local displacements
    q: SVector<f64, 6>,       // Total displacements
    v: SVector<f64, 6>,       // Total velocities
    f: SVector<f64, 3>,       // Linear forces
    Q: SVector<f64, 6>,       // Nonlinear forces
}

impl BeamElement {
    // Number of nodes and Gauss integration points
    pub const N_NODES: usize = 4;
    pub const N_GAUSS: usize = 3;

    // Divides the given curve into a number of equally spaced elements.
    // Returns a list of elements as well as the arc lengths, positions and angles of the nodes.
    // TODO: Return values s_nodes, u_node might not be needed if the evaluation works properly
    pub fn discretize<C, S>(curve: &C, section: &S, s_eval: &[f64], n_elements: usize) -> (Vec<LinearBeamSegment>, Vec<f64>, Vec<SVector<f64, 3>>)
        where C: PlanarCurve,
              S: CrossSection
    {
        // TODO: Assert that s_eval is sorted and in range of the curve

        let s_nodes = lin_space(curve.s_start()..=curve.s_end(), n_elements + 1).collect_vec();
        let u_nodes = s_nodes.iter().map(|&s| {
            let position = curve.position(s);
            vector![position[0], position[1], curve.angle(s)]
        }).collect_vec();

        // TODO: Better solution for numerical problems?
        let tolerance = 1e-9;

        let segments = s_nodes.iter().tuple_windows().enumerate().map(|(i, (&s0, &s1))| {
            // TODO: More efficient implementation than filtering each time
            let s_eval = if i == 0 {
                s_eval.iter().copied().filter(|&s| s >= s0 - tolerance && s <= s1 ).collect_vec()    // Include left boundary with tolerance
            }
            else if i == n_elements - 1 {
                s_eval.iter().copied().filter(|&s| s > s0 && s <= s1 + tolerance ).collect_vec()    // Include right boundary with tolerance
            }
            else {
                s_eval.iter().copied().filter(|&s| s > s0 && s <= s1 ).collect_vec()    // Exclude left boundary
            };

            LinearBeamSegment::new(curve, section, s0, s1, &s_eval)
        }).collect();

        (segments, s_nodes, u_nodes)
    }

    // Creates an element on the given curve between two arc lengths.
    // Returns the element as well as the arc lengths, positions and angles of the nodes.
    pub fn new(segment: &LinearBeamSegment) -> Self {
        // Transform stiffness matrix to moving reference frame

        let dx = segment.p1[0] - segment.p0[0];
        let dy = segment.p1[1] - segment.p0[1];

        let l0 = f64::hypot(dx, dy);
        let a0 = f64::atan2(dy, dx);
        let R0 = matrix![
            f64::cos(a0), -f64::sin(a0), 0.0;
            f64::sin(a0), f64::cos(a0), 0.0;
            0.0, 0.0, 1.0;
        ];

        let tx = dx/l0;
        let ty = dy/l0;

        let T = matrix![
            0.0, 0.0, 0.0;
            0.0, 0.0, 0.0;
            0.0, 1.0, 0.0;
            tx , 0.0, 0.0;
            ty , 0.0, 0.0;
            0.0, 0.0, 1.0;
        ];

        // Transform stiffness matrix
        let K = T.transpose()*segment.K*T;

        // Transform initial positions
        let p0 = vector![segment.p0[0], segment.p0[1], a0];
        let pe = segment.pe.iter().map(|&pe| R0.transpose()*(pe - p0)).collect();

        let u_eval = segment.u_eval.iter().map(|E| E*T ).collect();

        let M = segment.M;

        Self {
            M,
            K,
            s0: segment.s0,
            s1: segment.s1,
            se: segment.se.clone(),
            pe,
            l0,
            β0: segment.p0[2] - a0,
            β1: segment.p1[2] - a0,
            u_eval,
            C_inv: segment.C_inv.clone(),
            u: Default::default(),
            q: Default::default(),
            v: Default::default(),
            f: Default::default(),
            Q: Default::default(),
        }
    }

    pub fn eval_lengths(&self) -> &[f64] {
        &self.se
    }

    pub fn eval_positions(&self) -> impl Iterator<Item=SVector<f64, 3>> + '_ {
        // Local transformation
        // TODO: Redundant computations, store when evaluating forces
        let dx = self.q[3] - self.q[0];
        let dy = self.q[4] - self.q[1];
        let a0 = f64::atan2(dy, dx);

        let p0 = vector![
            self.q[0],
            self.q[1],
            a0
        ];

        let R = matrix![
            f64::cos(a0), -f64::sin(a0), 0.0;
            f64::sin(a0), f64::cos(a0), 0.0;
            0.0, 0.0, 1.0;
        ];

        self.se.iter().enumerate().map(move |(i, _)| {
            p0 + R*(self.pe[i] + self.u_eval[i]*self.u)
        })
    }

    // TODO: Duplicate calculation with eval_positions(), maybe combine
    pub fn eval_forces(&self) -> impl Iterator<Item=SVector<f64, 3>> + '_ {
        let Fx = self.Q[3];
        let Fy = self.Q[4];
        let Mz = self.Q[5];

        let x1 = self.q[3];
        let y1 = self.q[4];

        self.eval_positions().map(move |p| {
            let x = p[0];
            let y = p[1];
            let φ = p[2];

            let N = Fx*f64::cos(φ) + Fy*f64::sin(φ);
            let Q = Fy*f64::cos(φ) - Fx*f64::sin(φ);
            let M = Fy*(x1 - x) - Fx*(y1 - y) + Mz;

            vector![N, M, Q]
        })
    }

    // TODO: Duplicate calculation with eval_forces(), maybe combine
    pub fn eval_strains(&self) -> impl Iterator<Item=SVector<f64, 3>> + '_ {
        self.eval_forces().enumerate().map(|(i, f)| {
            self.C_inv[i]*f
        })
    }
}

impl Element for BeamElement {
    fn evaluate_mass_matrix(&self, M: &mut VectorView) {
        M.add(self.M);
    }

    fn set_state_and_evaluate(&mut self, u: &PositionView, v: &VelocityView, mut q: Option<&mut VectorView>, mut K: Option<&mut MatrixView>, mut _D: Option<&mut MatrixView>) {
        // Update element state, including strain and curvature
        //self.u = u.get();
        self.v = v.get();
        self.q = u.get();

        let dx = u.at(3) - u.at(0);
        let dy = u.at(4) - u.at(1);

        let l = f64::hypot(dx, dy);
        let a = f64::atan2(dy, dx);

        let dl = l - self.l0;
        let dφ0 = normalize_angle(u.at(2) - a - self.β0);
        let dφ1 = normalize_angle(u.at(5) - a - self.β1);

        self.u = vector![dl, dφ0, dφ1];

        if q.is_some() || K.is_some() {
            let f = self.K*self.u;
            self.f = f; // TODO

            let J = matrix![
                -dx/l    , -dy/l    , 0.0, dx/l    ,  dy/l    , 0.0;
                -dy/(l*l),  dx/(l*l), 1.0, dy/(l*l), -dx/(l*l), 0.0;
                -dy/(l*l),  dx/(l*l), 0.0, dy/(l*l), -dx/(l*l), 1.0;
            ];

            self.Q = J.transpose()*f;

            // Compute elastic forces if needed
            if let Some(ref mut q) = q {
                q.add(J.transpose()*f);
            }

            // Compute stiffness matrix if needed
            if let Some(ref mut K) = K {
                let c0 = dx*dx/l.powi(3) - 1.0/l;
                let c1 = dy*dy/l.powi(3) - 1.0/l;
                let c2 = dx*dy/l.powi(3);
                let c3 = 2.0*dx*dx/l.powi(4) - 1.0/l.powi(2);
                let c4 = 2.0*dy*dy/l.powi(4) - 1.0/l.powi(2);
                let c5 = 2.0*dx*dy/l.powi(4);

                let dJ0 = matrix![
                    -c0, -c2, 0.0, c0,  c2, 0.0;
                    -c5,  c3, 0.0, c5, -c3, 0.0;
                    -c5,  c3, 0.0, c5, -c3, 0.0;
                ];

                let dJ1 = matrix![
                    -c2, -c1, 0.0, c2,  c1, 0.0;
                    -c4,  c5, 0.0, c4, -c5, 0.0;
                    -c4,  c5, 0.0, c4, -c5, 0.0;
                ];

                let Kn: SMatrix<f64, 6, 6> = stack![dJ0.transpose()*f, dJ1.transpose()*f, SVector::<f64, 6>::zeros(), -dJ0.transpose()*f, -dJ1.transpose()*f, SVector::<f64, 6>::zeros()];
                K.add(&(Kn + J.transpose()*self.K*J));
            }
        }
    }

    fn potential_energy(&self) -> f64 {
        return 0.5*self.u.dot(&(self.K*self.u));
    }

    fn kinetic_energy(&self) -> f64 {
        return 0.5*self.v.dot(&(self.M.component_mul(&self.v)));
    }
}

#[cfg(test)]
mod tests {
    use std::ops::AddAssign;
    use iter_num_tools::lin_space;
    use nalgebra::{DMatrix, matrix, SMatrix, stack, vector};
    use crate::fem::elements::beam::{CrossSection, LinearBeamSegment, PlanarCurve};
    use crate::bow::sections::section::{LayerAlignment, LayeredCrossSection};
    use crate::bow::input::{Layer, Material, Width};
    use crate::bow::profile::profile::CurvePoint;
    use crate::bow::profile::segments::clothoid::{ArcInput, ClothoidSegment, LineInput};

    #[test]
    fn test_linear_stiffness_matrix_straight() {
        // Computes the stiffness matrix of a straight beam segment with constant cross section
        // and an arbitrary starting point and -angle and compares it to the exact solution
        // as well as the fem approximation.

        let length = 0.8;
        let alpha = 0.1;
        let start = CurvePoint::new(0.0, alpha, vector![1.5, 2.0]);
        let curve = ClothoidSegment::line(&start, &LineInput{ length });

        let material = Material::new("Steel", "", 7850.0, 210e9, 80e9);
        let width = Width::new(vec![[0.0, 0.01], [1.0, 0.01]]);
        let layer = Layer::new("", 0, vec![[0.0, 0.01], [1.0, 0.01]]);
        let section = LayeredCrossSection::new(curve.length(), &width, &vec![layer], &vec![material], LayerAlignment::SectionCenter).expect("Failed to construct cross section");

        let n_elements = 100;
        let segment_fem = LinearBeamSegmentFEM::new(&curve, &section, 0.0, curve.length(), n_elements);
        let segment_num = LinearBeamSegment::new(&curve, &section, 0.0, curve.length(), &segment_fem.s_eval);

        let C_sec = section.C(0.0);
        let K_ref = LinearBeamSegmentFEM::element_stiffness_matrix(C_sec[(0, 0)], C_sec[(1, 1)], C_sec[(2, 2)], curve.length(), alpha);

        println!("Error K_num = {}", (segment_num.K - K_ref).component_div(&K_ref));
        println!("Error K_fem = {}", (segment_num.K - K_ref).component_div(&K_ref));

        // Check stiffness matrices
        assert_relative_eq!(segment_num.K, K_ref, max_relative=1e-9);
        assert_relative_eq!(segment_fem.K, K_ref, max_relative=1e-6);

        // Check displacement evaluation matrices
        for i in 0..=n_elements {
            assert_abs_diff_eq!(segment_fem.u_eval[i], segment_num.u_eval[i], epsilon=1e-6);
        }
    }

    #[test]
    fn test_linear_stiffness_matrix_curved() {
        // Computes the stiffness matrix of a curved beam segment with constant cross section
        // and an arbitrary starting point and -angle and compares it to the fem approximation.

        let start = CurvePoint::new(0.0, 0.1, vector![1.5, 2.0]);
        let curve = ClothoidSegment::arc(&start, &ArcInput{ length: 0.8, radius: 0.4 });

        let material = Material::new("Steel", "", 7850.0, 210e9, 80e9);
        let width = Width::new(vec![[0.0, 0.01], [1.0, 0.01]]);
        let layer = Layer::new("", 0, vec![[0.0, 0.01], [1.0, 0.01]]);
        let section = LayeredCrossSection::new(curve.length(), &width, &vec![layer], &vec![material], LayerAlignment::SectionCenter).expect("Failed to construct cross section");

        let n_elements = 100;
        let segment_fem = LinearBeamSegmentFEM::new(&curve, &section, 0.0, curve.length(), n_elements);
        let segment_num = LinearBeamSegment::new(&curve, &section, 0.0, curve.length(), &segment_fem.s_eval);

        // Check stiffness matrices
        assert_relative_eq!(segment_fem.K, segment_num.K, max_relative=1e-3);    // TODO: Precision?

        // Check displacement evaluation matrices
        for i in 0..=n_elements {
            assert_abs_diff_eq!(segment_fem.u_eval[i], segment_num.u_eval[i], epsilon=1e-3);    // TODO: Precision?
        }
    }

    #[test]
    fn test_linear_stiffness_matrix_curved_nonuniform() {
        // Computes the stiffness matrix of a curved beam segment with varying cross section
        // and an arbitrary starting point and -angle and compares it to the fem approximation.

        let start = CurvePoint::new(0.0, 0.1, vector![1.5, 2.0]);
        let curve = ClothoidSegment::arc(&start, &ArcInput{ length: 0.8, radius: 0.4 });

        let material = Material::new("Steel", "", 7850.0, 210e9, 80e9);
        let width = Width::new(vec![[0.0, 0.01], [1.0, 0.005]]);
        let layer = Layer::new("", 0, vec![[0.0, 0.01], [1.0, 0.005]]);
        let section = LayeredCrossSection::new(curve.length(), &width, &vec![layer], &vec![material], LayerAlignment::SectionCenter).expect("Failed to construct cross section");

        let n_elements = 100;
        let segment_fem = LinearBeamSegmentFEM::new(&curve, &section, 0.0, curve.length(), n_elements);
        let segment_num = LinearBeamSegment::new(&curve, &section, 0.0, curve.length(), &segment_fem.s_eval);

        // Check stiffness matrices
        assert_relative_eq!(segment_fem.K, segment_num.K, max_relative=1e-3);    // TODO: Precision?

        // Check displacement evaluation matrices
        for i in 0..=n_elements {
            assert_abs_diff_eq!(segment_fem.u_eval[i], segment_num.u_eval[i], epsilon=1e-3);    // TODO: Precision?
        }
    }

    // Approximates the stiffness matrix of a curved, non-uniform beam segment by using a number of straight elements
    // and reducing the resulting stiffness matrix to the relevant displacements (first and last node)

    pub struct LinearBeamSegmentFEM {
        K: SMatrix<f64, 6, 6>,
        s_eval: Vec<f64>,
        u_eval: Vec<SMatrix<f64, 3, 6>>
    }

    impl LinearBeamSegmentFEM {
        pub fn new<C, S>(curve: &C, section: &S, s0: f64, s1: f64, n_elements: usize) -> Self
            where C: PlanarCurve,
                  S: CrossSection
        {
            assert!(n_elements >= 2);

            let n_nodes = n_elements + 1;
            let n_dofs = 3*n_nodes;

            // Create total stiffness matrix for all nodes
            let mut K_full = DMatrix::zeros(n_dofs, n_dofs);

            let s: Vec<f64> = lin_space(s0..=s1, n_nodes).collect();
            for i in 0..n_elements {
                let r_prev = curve.position(s[i]);
                let r_next = curve.position(s[i+1]);

                let dx = r_next[0] - r_prev[0];
                let dy = r_next[1] - r_prev[1];

                // TODO: Assert that C are diagonal matrices
                let C_prev = section.C(s[i]);
                let C_next = section.C(s[i+1]);

                K_full.view_mut((3*i, 3*i), (6, 6)).add_assign(&Self::element_stiffness_matrix(
                    0.5*(C_prev[(0, 0)] + C_next[(0, 0)]),    // Average longitudinal stiffness
                    0.5*(C_prev[(1, 1)] + C_next[(1, 1)]),    // Average bending stiffness
                    0.5*(C_prev[(2, 2)] + C_next[(2, 2)]),    // Average shear stiffness
                    f64::hypot(dx, dy),                               // Element length
                    f64::atan2(dy, dx)                                // Element angle
                ));
            }

            /*
            Reducing the total stiffness matrix to forces and displacements of the first and last node.
            K_total is partitioned like this:

            | F1 |   | K11    K12    K13 |   | U1 |
            | 0  | = | K12^T  K22    K23 | * | U2 |
            | F3 |   | K13^T  K23^T  K33 |   | U3 |

            F1, U1: Forces and displacements of the first node
            F3, U3: Forces and displacements of the last node
            U2: Equilibrium displacements of all nodes inbetween.

            Second block equation:

            0 = K12^T*U1 + K22*U2 + K23*U3
              => U2 = -K22^(-1)*(K12^T*U1 + K23*U3)

            First and third equations:

            F1 = K11*U1 + K12*U2 + K13*U3
               = K11*U1 - K12*K22^(-1)*(K12^T*U1 + K23*U3) + K13*U3
               = (K11 - K12*K22^(-1)*K12^T)*U1 + (K13 - K12*K22^(-1)*K23)*U3

            F3 = K13^T*U1 + K23^T*U2 + K33*U3
               = K13^T*U1 - K23^T*K22^(-1)*(K12^T*U1 + K23*U3) + K33*U3
               = (K13^T - K23^T*K22^(-1)*K12^T)*U1 + (K33 - K23^T*K22^(-1)*K23)*U3

            Giving the reduced stiffness matrix:

            | F1 |   | K11 - K12*K22^(-1)*K12^T      K13 - K12*K22^(-1)*K23   |   | U1 |
            |    | = |                                                        | * |    |
            | F3 |   | K13^T - K23^T*K22^(-1)*K12^T  K33 - K23^T*K22^(-1)*K23 |   | U3 |
            */

            let K11 = K_full.view((0, 0), (3, 3));
            let K22 = K_full.view((3, 3), (n_dofs-6, n_dofs-6));
            let K33 = K_full.view((n_dofs - 3, n_dofs - 3), (3, 3));
            let K12 = K_full.view((0, 3), (3, n_dofs - 6));
            let K13 = K_full.view((0, n_dofs - 3), (3, 3));
            let K23 = K_full.view((3, n_dofs - 3), (n_dofs - 6, 3));

            let K22_inv = K22.try_inverse().expect("Failed to invert stiffness matrix");

            // TODO: Find a way to use stack![] here with a fixed size matrix as a result.
            let mut K_red = SMatrix::<f64, 6, 6>::zeros();
            K_red.fixed_view_mut::<3, 3>(0, 0).copy_from(&(K11 - K12*&K22_inv*K12.transpose()));
            K_red.fixed_view_mut::<3, 3>(0, 3).copy_from(&(K13 - K12*&K22_inv*K23));
            K_red.fixed_view_mut::<3, 3>(3, 0).copy_from(&(K13.transpose()-K23.transpose()*&K22_inv*K12.transpose()));
            K_red.fixed_view_mut::<3, 3>(3, 3).copy_from(&(K33-K23.transpose()*&K22_inv*K23));

            // Evaluation matrices for displacements of eliminated nodes
            let mut u_eval = Vec::new();

            // The displacement at s = s0 is equal to that of the left node
            u_eval.push(matrix![
                1.0, 0.0, 0.0, 0.0, 0.0, 0.0;
                0.0, 1.0, 0.0, 0.0, 0.0, 0.0;
                0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
            ]);

            // The displacements inbetween are determined by the equation for U2
            let eval = stack![-&K22_inv*&K12.transpose(), -&K22_inv*&K23];

            for i in 0..n_nodes-2 {
                u_eval.push(eval.fixed_view::<3, 6>(3*i, 0).into());
            }

            // The displacement at s = s1 is equal to that of the right node
            u_eval.push(matrix![
                0.0, 0.0, 0.0, 1.0, 0.0, 0.0;
                0.0, 0.0, 0.0, 0.0, 1.0, 0.0;
                0.0, 0.0, 0.0, 0.0, 0.0, 1.0;
            ]);

            Self {
                K: K_red,
                s_eval: s,
                u_eval: u_eval
            }
        }

        // Returns the analytical stiffness matrix of a straight Euler-Bernoulli beam segment
        // with constant cross section and a rotation angle alpha against the x-axis.
        // TODO: Add source for the matrices K and T below
        fn element_stiffness_matrix(EA: f64, EI: f64, GA: f64, l: f64, alpha: f64) -> SMatrix<f64, 6, 6> {
            let Φ = 12.0*EI/(GA*l*l);
            let K = matrix![
            EA/l,                          0.0,                        0.0, -EA/l,                          0.0,                        0.0;
             0.0,    12.0*EI/(l*l*l*(1.0 + Φ)),     6.0*EI/(l*l*(1.0 + Φ)),   0.0,   -12.0*EI/(l*l*l*(1.0 + Φ)),     6.0*EI/(l*l*(1.0 + Φ));
             0.0,       6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(4.0 + Φ)/(1.0 + Φ),   0.0,      -6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(2.0 - Φ)/(1.0 + Φ);
           -EA/l,                          0.0,                        0.0,  EA/l,                          0.0,                        0.0;
             0.0,   -12.0*EI/(l*l*l*(1.0 + Φ)),    -6.0*EI/(l*l*(1.0 + Φ)),   0.0,    12.0*EI/(l*l*l*(1.0 + Φ)),    -6.0*EI/(l*l*(1.0 + Φ));
             0.0,       6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(2.0 - Φ)/(1.0 + Φ),   0.0,      -6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(4.0 + Φ)/(1.0 + Φ);
        ];

            let T = matrix![
            f64::cos(alpha), f64::sin(alpha), 0.0,              0.0,             0.0, 0.0;
           -f64::sin(alpha), f64::cos(alpha), 0.0,              0.0,             0.0, 0.0;
                        0.0,             0.0, 1.0,              0.0,             0.0, 0.0;
                        0.0,             0.0, 0.0,  f64::cos(alpha), f64::sin(alpha), 0.0;
                        0.0,             0.0, 0.0, -f64::sin(alpha), f64::cos(alpha), 0.0;
                        0.0,             0.0, 0.0,              0.0,             0.0, 1.0;
        ];

            T.transpose()*K*T
        }
    }
}