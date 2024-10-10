use nalgebra::{SVector, vector};

use crate::fem::system::{element::Element, views::{MatrixView, PositionView, VelocityView, VectorView}};

pub struct MassElement {
    M: SVector<f64, 3>,    // Mass matrix
    v: SVector<f64, 3>,    // Current velocity
}

impl MassElement {
    pub fn new(m: f64) -> Self {
        let mut element = Self {
            M: SVector::zeros(),
            v: SVector::zeros()
        };

        element.set_mass(m);
        element
    }

    pub fn set_mass(&mut self, m: f64) {
        self.M = vector![m, m, 0.0];
    }
}

impl Element for MassElement {
    fn evaluate_mass_matrix(&self, M: &mut VectorView) {
        M.add_vec(self.M);
    }

    fn set_state_and_evaluate(&mut self, _u: &PositionView, v: &VelocityView, _q: Option<&mut VectorView>, _K: Option<&mut MatrixView>, _D: Option<&mut MatrixView>) {
        self.v = v.get();
    }

    fn potential_energy(&self) -> f64 {
        0.0
    }

    fn kinetic_energy(&self) -> f64 {
        0.5*&self.v.dot(&(&self.M.component_mul(&self.v)))
    }
}