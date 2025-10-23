use nalgebra::{matrix, vector, SMatrix, SVector, stack};
use crate::fem::elements::beam::linear::LinearBeamSegment;
use crate::fem::system::element::Element;
use crate::fem::system::views::{MatrixView, PositionView, VectorView, VelocityView};
use crate::utils::functions::normalize_angle;

pub struct BeamElement {
    // Constant data
    M: SVector<f64, 6>,       // Element mass matrix
    K: SMatrix<f64, 3, 3>,    // Local stiffness matrix
    D: SMatrix<f64, 3, 3>,    // Local damping matrix

    se: Vec<f64>,              // Evaluation lengths
    pe: Vec<SVector<f64, 3>>,  // Initial positions wrt. local reference frame

    l0: f64,                  // Initial length along x axis
    β0: f64,                  // Angular offset at left node
    β1: f64,                  // Angular offset at right node

    u_eval: Vec<SMatrix<f64, 3, 3>>,    // Displacement evaluation
    C_inv: Vec<SMatrix<f64, 3, 3>>,     // Inverse cross-section stiffness

    // State dependent data

    u: SVector<f64, 6>,     // Total displacements
    v: SVector<f64, 6>,     // Total velocities
    ul: SVector<f64, 3>,    // Local displacements
    vl: SVector<f64, 3>,    // Local velocities
    fe: SVector<f64, 3>,    // Local elastic forces
    fd: SVector<f64, 3>,    // Local damping forces
    Qe: SVector<f64, 6>,    // Total elastic forces
    Qd: SVector<f64, 6>,    // Total damping forces
}

pub struct EvalResult {
    pub length: f64,                  // Length at which the element was evaluated
    pub position: SVector<f64, 3>,    // Cross section position and orientation [x, y, φ]
    pub velocity: SVector<f64, 3>,    // Velocity of the cross section position [vx, vy, vφ]
    pub forces: SVector<f64, 3>,      // Cross section forces [N, Q, M]
    pub strains: SVector<f64, 3>      // Cross section strains [ε, γ, κ]
}

impl BeamElement {
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

        let u_eval = segment.Ep.iter().map(|E| E*T ).collect();

        let M = segment.M;

        Self {
            M,
            K,
            D: SMatrix::zeros(),
            se: segment.se.clone(),
            pe,
            l0,
            β0: segment.p0[2] - a0,
            β1: segment.p1[2] - a0,
            u_eval,
            C_inv: segment.Ci.clone(),
            u: SVector::zeros(),
            v: SVector::zeros(),
            ul: SVector::zeros(),
            vl: SVector::zeros(),
            fe: SVector::zeros(),
            fd: SVector::zeros(),
            Qe: SVector::zeros(),
            Qd: SVector::zeros(),
        }
    }

    pub fn set_damping(&mut self, alpha: f64) {
        assert!(alpha >= 0.0, "Damping coefficient must not be negative");
        self.D = alpha*self.K;
    }

    pub fn eval_lengths(&self) -> &[f64] {
        &self.se
    }

    pub fn eval_properties(&self) -> impl Iterator<Item=EvalResult> + '_ {
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

        let dadu = vector![dy, -dx, 0.0, -dy, dx, 0.0]/(dx*dx + dy*dy);
        let dadt = dadu.dot(&(self.v));

        let p0_dot = vector![
            self.v[0],
            self.v[1],
            dadt
        ];

        let dRda = matrix![
            -f64::sin(a0), -f64::cos(a0), 0.0;
            f64::cos(a0), -f64::sin(a0), 0.0;
            0.0, 0.0, 0.0;
        ];

        let Fx = self.Qe[3];
        let Fy = self.Qe[4];
        let Mz = self.Qe[5];

        let x1 = self.u[3];
        let y1 = self.u[4];

        self.se.iter().enumerate().map(move |(i, _)| {
            let length = self.se[i];
            let position = p0 + R*(self.pe[i] + self.u_eval[i]*self.ul);
            let velocity = p0_dot + dadt*dRda*(self.pe[i] + self.u_eval[i]*self.ul) + R*self.u_eval[i]*self.vl;

            let x = position[0];
            let y = position[1];
            let φ = position[2];

            let N = Fx*f64::cos(φ) + Fy*f64::sin(φ);
            let Q = Fy*f64::cos(φ) - Fx*f64::sin(φ);
            let M = Fy*(x1 - x) - Fx*(y1 - y) + Mz;

            let forces = vector![N, Q, M];
            let strains = self.C_inv[i]*forces;

            EvalResult {
                length,
                position,
                velocity,
                forces,
                strains,
            }
        })
    }
}

impl Element for BeamElement {
    fn evaluate_mass_matrix(&self, M: &mut VectorView) {
        M.add_vec(self.M);
    }

    fn update_and_evaluate(&mut self, u: &PositionView, v: &VelocityView, mut q: Option<&mut VectorView>, mut K: Option<&mut MatrixView>, mut D: Option<&mut MatrixView>) {
        // Update element state
        self.u = u.get();
        self.v = v.get();

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

        if q.is_some() || K.is_some() || D.is_some() {
            self.fe = self.K*self.ul;      // Local elastic forces
            self.fd = self.D*self.vl;      // Local damping forces
            let ft = self.fe + self.fd;    // Total local forces

            self.Qe = J.transpose()*self.fe;
            self.Qd = J.transpose()*self.fd;

            // Compute elastic forces if needed
            if let Some(ref mut q) = q {
                q.add_vec(self.Qe + self.Qd);
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

                K.add_mat(&(
                    Kk + Kd + J.transpose()*self.K*J
                ));
            }

            // Compute damping matrix if needed
            if let Some(ref mut D) = D {
                D.add_mat(&(J.transpose()*self.D*J));
            }
        }
    }

    fn potential_energy(&self) -> f64 {
        0.5*self.ul.dot(&(self.K*self.ul))
    }

    fn kinetic_energy(&self) -> f64 {
        0.5*self.v.dot(&(self.M.component_mul(&self.v)))
    }

    fn dissipative_power(&self) -> f64 {
        self.fd.dot(&self.vl)
    }
}