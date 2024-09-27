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
use crate::fem::elements::beam::geometry::{CrossSection, PlanarCurve};
use crate::fem::elements::beam::linear::LinearBeamSegment;
use crate::fem::system::element::Element;
use crate::fem::system::views::{MatrixView, PositionView, VectorView, VelocityView};
use crate::numerics::functions::normalize_angle;

pub struct BeamElement {
    // Constant data
    M: SVector<f64, 6>,       // Element mass matrix
    K: SMatrix<f64, 3, 3>,    // Linear stiffness matrix
    D: SMatrix<f64, 3, 3>,    // Linear stiffness matrix

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

    u: SVector<f64, 6>,    // Total displacements
    v: SVector<f64, 6>,    // Total velocities
    ul: SVector<f64, 3>,   // Local displacements
    vl: SVector<f64, 3>,   // Local velocities
    Qe: SVector<f64, 6>,    // Total elastic forces
    Qd: SVector<f64, 6>,    // Total damping forces
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

        let u_eval = segment.ue.iter().map(|E| E*T ).collect();

        let M = segment.M;

        Self {
            M,
            K,
            D: SMatrix::zeros(),
            s0: segment.s0,
            s1: segment.s1,
            se: segment.se.clone(),
            pe,
            l0,
            β0: segment.p0[2] - a0,
            β1: segment.p1[2] - a0,
            u_eval,
            C_inv: segment.Ci.clone(),
            u: Default::default(),
            v: Default::default(),
            ul: Default::default(),
            vl: Default::default(),
            Qe: Default::default(),
            Qd: Default::default(),
        }
    }

    pub fn set_damping(&mut self, alpha: f64) {
        self.D = alpha*self.K;
    }

    pub fn eval_lengths(&self) -> &[f64] {
        &self.se
    }

    pub fn eval_positions(&self) -> impl Iterator<Item=SVector<f64, 3>> + '_ {
        // Local transformation
        // TODO: Redundant computations, store when evaluating forces
        let dx = self.u[3] - self.u[0];
        let dy = self.u[4] - self.u[1];
        let a0 = f64::atan2(dy, dx);

        let p0 = vector![
            self.u[0],
            self.u[1],
            a0
        ];

        let R = matrix![
            f64::cos(a0), -f64::sin(a0), 0.0;
            f64::sin(a0), f64::cos(a0), 0.0;
            0.0, 0.0, 1.0;
        ];

        self.se.iter().enumerate().map(move |(i, _)| {
            p0 + R*(self.pe[i] + self.u_eval[i]*self.ul)
        })
    }

    // TODO: Duplicate calculation with eval_positions(), maybe combine
    pub fn eval_forces(&self) -> impl Iterator<Item=SVector<f64, 3>> + '_ {
        let Fx = self.Qe[3];
        let Fy = self.Qe[4];
        let Mz = self.Qe[5];

        let x1 = self.u[3];
        let y1 = self.u[4];

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

    fn set_state_and_evaluate(&mut self, u: &PositionView, v: &VelocityView, mut q: Option<&mut VectorView>, mut K: Option<&mut MatrixView>, mut D: Option<&mut MatrixView>) {
        // Update element state, including strain and curvature
        //self.u = u.get();
        self.v = v.get();
        self.u = u.get();

        let dx = u.at(3) - u.at(0);
        let dy = u.at(4) - u.at(1);

        let l = f64::hypot(dx, dy);
        let a = f64::atan2(dy, dx);

        let dl = l - self.l0;
        let dφ0 = normalize_angle(u.at(2) - a - self.β0);
        let dφ1 = normalize_angle(u.at(5) - a - self.β1);

        let J = matrix![
            -dx/l    , -dy/l    , 0.0, dx/l    ,  dy/l    , 0.0;
            -dy/(l*l),  dx/(l*l), 1.0, dy/(l*l), -dx/(l*l), 0.0;
            -dy/(l*l),  dx/(l*l), 0.0, dy/(l*l), -dx/(l*l), 1.0;
        ];

        self.ul = vector![dl, dφ0, dφ1];
        self.vl = J*self.v;

        if q.is_some() || K.is_some() {
            let fe = self.K*self.ul;    // Local elastic forces
            let fd = self.D*self.vl;    // Local damping forces
            let ft = fe + fd;           // Total local forces

            self.Qe = J.transpose()*fe;
            self.Qd = J.transpose()*fd;

            // Compute elastic forces if needed
            if let Some(ref mut q) = q {
                q.add(self.Qe + self.Qd);
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

                let Kk = stack![dJ0.transpose()*ft, dJ1.transpose()*ft, SVector::<f64, 6>::zeros(), -dJ0.transpose()*ft, -dJ1.transpose()*ft, SVector::<f64, 6>::zeros()];
                let Kd = J.transpose()*self.D*stack![dJ0*self.v, dJ1*self.v, SVector::<f64, 3>::zeros(), -dJ0*self.v, -dJ1*self.v, SVector::<f64, 3>::zeros()];

                K.add(&(
                    Kk + Kd + J.transpose()*self.K*J
                ));
            }

            // Compute damping matrix if needed
            if let Some(ref mut D) = D {
                D.add(&(J.transpose()*self.D*J));
            }
        }
    }

    fn potential_energy(&self) -> f64 {
        return 0.5*self.ul.dot(&(self.K*self.ul));
    }

    fn kinetic_energy(&self) -> f64 {
        return 0.5*self.v.dot(&(self.M.component_mul(&self.v)));
    }
}