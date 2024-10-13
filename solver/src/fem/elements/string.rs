use itertools::Itertools;
use nalgebra::{DMatrix, DVector, SVector, vector};
use crate::fem::system::element::Element;
use crate::fem::system::views::{MatrixView, PositionView, VectorView, VelocityView};
use crate::numerics::geometry::{convex_envelope, Orientation};

pub struct StringElement {
    // Constant data
    EA: f64,              // Linear stiffness
    ηA: f64,            // Linear damping
    l0: f64,              // Initial length
    offsets: Vec<f64>,    // Distances between limb nodes and contact points

    // State data
    points: Vec<SVector<f64, 2>>,    // String center + surface points of the limb elements
    angles: Vec<f64>,                // Angles of the points
    indices: Vec<usize>,             // Indices of the points that are in contact
    lengths: Vec<f64>,               // Lengths between the contact points
    lt: f64,                          // Total length of the string
    dldu: DVector<f64>,              // Gradient of the length wrt to the displacements
    dldu2: DMatrix<f64>,             // Hessian of the length wrt to the displacements
    Nt: f64,                          // Total normal force
    Ne: f64,                         // Elastic component of the normal force
    Nv: f64,                         // Viscous component of the normal force
}

impl StringElement {
    pub fn new(EA: f64, ηA: f64, l0: f64, offsets: Vec<f64>) -> Self {
        let n_points = offsets.len();

        Self {
            EA,
            ηA,
            l0,
            offsets,
            points: vec![SVector::zeros(); n_points],   // Size: Number of nodes,
            angles: vec![0.0; n_points],                // Size: Number of nodes,
            indices: Vec::with_capacity(n_points),      // Size: Initially zero, maximum is number of nodes
            lengths: Vec::with_capacity(n_points - 1),  // Size: Initially zero, maximum is number of nodes - 1
            lt: l0,
            dldu: DVector::zeros(3*n_points),                 // Size: Number of points times number of dofs per point
            dldu2: DMatrix::zeros(3*n_points, 3*n_points),    // Size: Number of points times number of dofs per point
            Nt: 0.0,
            Ne: 0.0,
            Nv: 0.0,
        }
    }

    // Creates a string element between two points without offsets, equivalent to a bar element
    pub fn bar(EA: f64, ηA: f64, l0: f64) -> Self {
        Self::new(EA, ηA, l0, vec![0.0; 2])
    }

    // Creates a bar with spring parameters, i.e. mass (total), damping and stiffness instead of cross section properties
    pub fn spring(k: f64, d: f64, l0: f64) -> Self {
        Self::bar(k*l0, d*l0, l0)
    }

    pub fn get_initial_length(&self) -> f64 {
        self.l0
    }

    pub fn set_initial_length(&mut self, l0: f64) {
        self.l0 = l0;
    }

    pub fn set_linear_damping(&mut self, ηA: f64) {
        self.ηA = ηA;
    }

    // Total normal force
    pub fn normal_force(&self) -> f64 {
        self.Nt
    }

    // Elastic component of the normal force
    pub fn normal_force_elastic(&self) -> f64 {
        self.Ne
    }

    // Viscous component of the normal force
    pub fn normal_force_viscous(&self) -> f64 {
        self.Nv
    }

}

impl Element for StringElement {
    fn evaluate_mass_matrix(&self, _M: &mut VectorView) {
        // Element has no mass properties
    }

    fn set_state_and_evaluate(&mut self, u: &PositionView, v: &VelocityView, mut q: Option<&mut VectorView>, mut K: Option<&mut MatrixView>, mut D: Option<&mut MatrixView>) {
        assert_eq!(u.len()/3, self.offsets.len(), "Invalid number of offsets");

        // Step 1: Compute position of all surface points from nodal coordinates and offsets
        for i in 0..self.points.len() {
            let x = u.at(3*i + 0);
            let y = u.at(3*i + 1);
            let φ = u.at(3*i + 2);

            self.points[i] = vector![
                x - self.offsets[i]*f64::sin(φ),
                y + self.offsets[i]*f64::cos(φ)
            ];

            self.angles[i] = φ;
        }

        // Step 2: Determine which of the surface points are in contact with the string
        convex_envelope(&self.points, &mut self.indices, Orientation::LeftTurn);

        // Step 3: Compute partial lengths and total length from contact points
        self.lengths.clear();
        self.lengths.extend(self.indices.iter().tuple_windows().map(|(&i, &j)| (self.points[j] - self.points[i]).norm()));
        self.lt = self.lengths.iter().sum();

        // Step 4: Compute partial derivatives of the length as needed for Q, K and D

        self.dldu.fill(0.0);
        self.dldu2.fill(0.0);

        for k in 0..self.indices.len()-1 {
            let i = self.indices[k];
            let j = self.indices[k+1];

            let xi = self.points[i][0];
            let yi = self.points[i][1];
            let φi = self.angles[i];
            let di = self.offsets[i];

            let xj = self.points[j][0];
            let yj = self.points[j][1];
            let φj = self.angles[j];
            let dj = self.offsets[j];

            let lij = self.lengths[k];

            if q.is_some() || K.is_some() {
                let dl_dxi = (xi - xj)/lij;
                let dl_dyi = (yi - yj)/lij;
                let dl_dφi = -di*(dl_dxi*f64::cos(φi) + dl_dyi*f64::sin(φi));

                let dl_dxj = -dl_dxi;
                let dl_dyj = -dl_dyi;
                let dl_dφj = -dj*(dl_dxj*f64::cos(φj) + dl_dyj*f64::sin(φj));

                if q.is_some() || D.is_some() {
                    self.dldu[3*i+0] += dl_dxi;
                    self.dldu[3*i+1] += dl_dyi;
                    self.dldu[3*i+2] += dl_dφi;
                    self.dldu[3*j+0] += dl_dxj;
                    self.dldu[3*j+1] += dl_dyj;
                    self.dldu[3*j+2] += dl_dφj;
                }

                if K.is_some() {
                    let dl_dxi_dxi = (yj - yi).powi(2)/lij.powi(3);
                    let dl_dxi_dyi = -(xj - xi)*(yj - yi)/lij.powi(3);
                    let dl_dxi_dxj = -dl_dxi_dxi;
                    let dl_dxi_dyj = -dl_dxi_dyi;

                    let dl_dyi_dyi = (xj - xi).powi(2)/lij.powi(3);
                    let dl_dyi_dxj = -dl_dxi_dyi;
                    let dl_dyi_dyj = -dl_dyi_dyi;

                    let dl_dxj_dxj = dl_dxi_dxi;
                    let dl_dxj_dyj = dl_dxi_dyi;

                    let dl_dyj_dyj = dl_dyi_dyi;

                    let dl_dxi_dφi = -di*(dl_dxi_dxi*f64::cos(φi) + dl_dxi_dyi*f64::sin(φi));
                    let dl_dyi_dφi = -di*(dl_dyi_dyi*f64::sin(φi) + dl_dxi_dyi*f64::cos(φi));
                    let dl_dφi_dφi = -di*((dl_dxi_dφi + dl_dyi)*f64::cos(φi) + (dl_dyi_dφi - dl_dxi)*f64::sin(φi));

                    let dl_dxj_dφj = -dj*(dl_dxj_dxj*f64::cos(φj) + dl_dxj_dyj*f64::sin(φj));
                    let dl_dyj_dφj = -dj*(dl_dxj_dyj*f64::cos(φj) + dl_dyj_dyj*f64::sin(φj));
                    let dl_dφj_dφj = -dj*((dl_dxj_dφj + dl_dyj)*f64::cos(φj) + (dl_dyj_dφj - dl_dxj)*f64::sin(φj));

                    let dl_dxi_dφj = -dj*(dl_dxi_dxj*f64::cos(φj) + dl_dxi_dyj*f64::sin(φj));
                    let dl_dyi_dφj = -dj*(dl_dyi_dxj*f64::cos(φj) + dl_dyi_dyj*f64::sin(φj));
                    let dl_dφi_dφj = -di*(dl_dxi_dφj*f64::cos(φi) + dl_dyi_dφj*f64::sin(φi));

                    let dl_dxj_dφi = -di*(dl_dxi_dxj*f64::cos(φi) + dl_dxi_dyj*f64::sin(φi));
                    let dl_dyj_dφi = -di*(dl_dxi_dyj*f64::cos(φi) + dl_dyi_dyj*f64::sin(φi));

                    self.dldu2[(3*i+0, 3*i+0)] += dl_dxi_dxi; self.dldu2[(3*i+0, 3*i+1)] += dl_dxi_dyi; self.dldu2[(3*i+0, 3*i+2)] += dl_dxi_dφi; self.dldu2[(3*i+0, 3*j+0)] += dl_dxi_dxj; self.dldu2[(3*i+0, 3*j+1)] += dl_dxi_dyj; self.dldu2[(3*i+0, 3*j+2)] += dl_dxi_dφj;
                    self.dldu2[(3*i+1, 3*i+0)] += dl_dxi_dyi; self.dldu2[(3*i+1, 3*i+1)] += dl_dyi_dyi; self.dldu2[(3*i+1, 3*i+2)] += dl_dyi_dφi; self.dldu2[(3*i+1, 3*j+0)] += dl_dyi_dxj; self.dldu2[(3*i+1, 3*j+1)] += dl_dyi_dyj; self.dldu2[(3*i+1, 3*j+2)] += dl_dyi_dφj;
                    self.dldu2[(3*i+2, 3*i+0)] += dl_dxi_dφi; self.dldu2[(3*i+2, 3*i+1)] += dl_dyi_dφi; self.dldu2[(3*i+2, 3*i+2)] += dl_dφi_dφi; self.dldu2[(3*i+2, 3*j+0)] += dl_dxj_dφi; self.dldu2[(3*i+2, 3*j+1)] += dl_dyj_dφi; self.dldu2[(3*i+2, 3*j+2)] += dl_dφi_dφj;
                    self.dldu2[(3*j+0, 3*i+0)] += dl_dxi_dxj; self.dldu2[(3*j+0, 3*i+1)] += dl_dyi_dxj; self.dldu2[(3*j+0, 3*i+2)] += dl_dxj_dφi; self.dldu2[(3*j+0, 3*j+0)] += dl_dxj_dxj; self.dldu2[(3*j+0, 3*j+1)] += dl_dxj_dyj; self.dldu2[(3*j+0, 3*j+2)] += dl_dxj_dφj;
                    self.dldu2[(3*j+1, 3*i+0)] += dl_dxi_dyj; self.dldu2[(3*j+1, 3*i+1)] += dl_dyi_dyj; self.dldu2[(3*j+1, 3*i+2)] += dl_dyj_dφi; self.dldu2[(3*j+1, 3*j+0)] += dl_dxj_dyj; self.dldu2[(3*j+1, 3*j+1)] += dl_dyj_dyj; self.dldu2[(3*j+1, 3*j+2)] += dl_dyj_dφj;
                    self.dldu2[(3*j+2, 3*i+0)] += dl_dxi_dφj; self.dldu2[(3*j+2, 3*i+1)] += dl_dyi_dφj; self.dldu2[(3*j+2, 3*i+2)] += dl_dφi_dφj; self.dldu2[(3*j+2, 3*j+0)] += dl_dxj_dφj; self.dldu2[(3*j+2, 3*j+1)] += dl_dyj_dφj; self.dldu2[(3*j+2, 3*j+2)] += dl_dφj_dφj;
                }
            }
        }

        // Step 5: Compute generalized forces, stiffness- and damping matrices
        // Calculations are done in loops in order to avoid dynamic allocations from temporary DVector and DMatrix types.
        // This hasn't been benchmarked though, maybe the other way would be better.

        let dldt: f64 = self.dldu.iter().enumerate().map(|(i, dldu)| dldu*v.at(i)).sum();

        self.Ne = self.EA/self.l0 *(self.lt - self.l0);
        self.Nv = self.ηA /self.l0*dldt;
        self.Nt = self.Ne + self.Nv;

        if let Some(ref mut q) = q {
            for i in 0..self.dldu.len() {
                q.add(i, self.Nt*self.dldu[i]);
            }
        }

        if let Some(ref mut K) = K {
            for j in 0..self.dldu.len() {
                let dldt_du_j: f64 = self.dldu2.column(j).iter().enumerate().map(|(k, dldu2)| dldu2*v.at(k)).sum();
                for i in 0..self.dldu.len() {
                    K.add(i, j, self.EA/self.l0*self.dldu[j]*self.dldu[i] + self.ηA/self.l0*dldt_du_j*self.dldu[i] + self.Nt*self.dldu2[(i, j)]);
                }
            }
        }

        if let Some(ref mut D) = D {
            for i in 0..self.dldu.len() {
                for j in 0..self.dldu.len() {
                    D.add(i, j, self.ηA /self.l0*self.dldu[i]*self.dldu[j]);
                }
            }
        }
    }

    fn potential_energy(&self) -> f64 {
        0.5*self.Nt*(self.lt - self.l0)
        //self.lt
    }

    fn kinetic_energy(&self) -> f64 {
        0.0
    }
}