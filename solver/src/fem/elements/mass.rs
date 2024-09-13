use nalgebra::SVector;

use crate::fem::system::{element::Element, views::{MatrixView, PositionView, VelocityView, VectorView}};

pub struct MassElement {
    M: SVector<f64, 2>,    // Mass matrix
    v: SVector<f64, 2>,    // Current velocity
}

impl MassElement {
    pub fn new(m: f64) -> Self {
        Self {
            M: SVector::from_element(m),
            v: SVector::zeros()
        }
    }

    pub fn set_mass(&mut self, m: f64) {
        self.M = SVector::from_element(m);
    }
}

impl Element for MassElement {
    fn evaluate_mass_matrix(&self, M: &mut VectorView) {
        M.add(self.M);
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