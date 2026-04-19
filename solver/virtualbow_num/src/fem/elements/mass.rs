use nalgebra::SVector;

use crate::fem::system::{element::Element, views::{MatrixView, PositionView, VelocityView, VectorView}};

pub struct MassElement {
    M: SVector<f64, 3>,    // Mass matrix
    v: SVector<f64, 3>,    // Current velocity
}

impl MassElement {
    pub fn new(m: f64, J: f64) -> Self {
        let mut element = Self {
            M: SVector::zeros(),
            v: SVector::zeros()
        };

        element.set_mass(m);
        element.set_inertia(J);
        element
    }

    pub fn point(m: f64) -> Self {
        Self::new(m, 0.0)
    }

    pub fn get_mass(&self) -> f64 {
        self.M[0]
    }

    pub fn get_inertia(&self) -> f64 {
        self.M[2]
    }

    pub fn set_mass(&mut self, m: f64) {
        assert!(m >= 0.0, "Mass must not be negative");
        self.M[0] = m;
        self.M[1] = m;
    }

    pub fn set_inertia(&mut self, J: f64) {
        assert!(J >= 0.0, "Moment of inertia must not be negative");
        self.M[2] = J;
    }
}

impl Element for MassElement {
    fn evaluate_mass_matrix(&self, M: &mut VectorView) {
        M.add_vec(self.M);
    }

    fn update_and_evaluate(&mut self, _u: &PositionView, v: &VelocityView, _q: Option<&mut VectorView>, _K: Option<&mut MatrixView>, _D: Option<&mut MatrixView>) {
        self.v = v.get();
    }

    fn potential_energy(&self) -> f64 {
        0.0
    }

    fn kinetic_energy(&self) -> f64 {
        0.5*&self.v.dot(&self.M.component_mul(&self.v))
    }

    fn dissipative_power(&self) -> f64 {
        0.0
    }
}