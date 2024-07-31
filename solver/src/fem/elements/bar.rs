use nalgebra::{matrix, SMatrix, SVector};

use crate::fem::system::{element::Element, views::{MatrixView, PositionView, VelocityView, VectorView}};

pub struct BarElement {
    // Constant data
    rhoA: f64,    // Linear density
    etaA: f64,    // Linear damping
    EA: f64,      // Linear stiffness
    l0: f64,      // Initial length

    M: SVector<f64, 4>,

    // State dependent data
    v: SVector<f64, 4>,    // Current velocity
    l: f64,                // Current length
    N: f64,                // Current normal force
}

impl BarElement {
    pub fn new(rhoA: f64, etaA: f64, EA: f64, l0: f64) -> Self {
        Self {
            rhoA,
            etaA,
            EA,
            l0,
            M: SVector::<f64, 4>::from_element(0.5*rhoA*l0),
            v: SVector::zeros(),
            l: 0.0,
            N: 0.0
        }
    }

    // Construct bar with spring parameters, i.e. mass (total), damping and stiffness instead of cross section properties
    pub fn spring(m: f64, d: f64, k: f64, l0: f64) -> Self {
        Self::new(m/l0, d*l0, k*l0, l0)
    }

    pub fn set_initial_length(&mut self, l0: f64) {
        self.l0 = l0;
    }

    pub fn get_initial_length(&self) -> f64 {
        self.l0
    }

    pub fn normal_force(&self) -> f64 {
        self.N
    }
}

impl Element for BarElement {
    fn evaluate_mass_matrix(&self, M: &mut VectorView) {
        M.add(self.M);
    }

    fn set_state_and_evaluate(&mut self, u: &PositionView, v: &VelocityView, q: Option<&mut VectorView>, K: Option<&mut MatrixView>, D: Option<&mut MatrixView>) {
        // Update element state
        let dx = u.at(2) - u.at(0);
        let dy = u.at(3) - u.at(1);
        self.l = f64::hypot(dx, dy);
        self.v = v.get();

        // NEW

        let dS = 1.0/self.l0*matrix![
            1.0, 0.0, -1.0, 0.0;
            0.0, 1.0, 0.0, -1.0;
        ];

        let dSS = 1.0/self.l0.powi(2)*matrix![
            1.0,  0.0, -1.0,  0.0;
            0.0,  1.0,  0.0, -1.0;
           -1.0,  0.0,  1.0,  0.0;
            0.0, -1.0,  0.0,  1.0;
        ];

        let u = u.get::<4>();
        let v = v.get::<4>();

        let drds = &dS*&u;
        let drds_norm_sq = drds.norm_squared();
        let drds_norm = f64::sqrt(drds_norm_sq);

        let e = drds_norm - 1.0;

        let dedt = u.dot(&(dSS*v))/drds_norm;
        let dedu = dSS*u/drds_norm;

        let de_dudu = dS.transpose()*(SMatrix::<f64, 2, 2>::identity() - drds*drds.transpose()/drds_norm_sq)*dS/drds_norm;
        let de_dtdu = de_dudu*v;
        let de_dtdv = dS.transpose()*dS*u/drds_norm;

        self.N = self.EA*e + self.etaA*dedt;

        // Compute elastic forces, if needed
        if let Some(q) = q {
            q.add(self.N*self.l0*dedu);
        }

        // Compute tangent stiffness, if needed
        if let Some(K) = K {
            let dNdu = self.EA*dedu + self.etaA*de_dtdu;
            K.add(&(self.l0*dedu*dNdu.transpose() + self.l0*self.N*de_dudu));
        }

        // Compute damping matrix, if needed
        if let Some(D) = D {
            let dNdv = self.etaA*de_dtdv;
            D.add(&(self.l0*dNdv*dedu.transpose()));
        }
    }

    fn potential_energy(&self) -> f64 {
        0.5*self.N*(self.l - self.l0)
    }

    fn kinetic_energy(&self) -> f64 {
        0.25*self.rhoA*self.l0*self.v.dot(&self.v)
    }
}