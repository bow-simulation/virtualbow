use super::views::{MatrixView, PositionView, VelocityView, VectorView};
use downcast_rs::Downcast;

pub trait Element: Downcast {
    // Adds the element's contribution to the global mass matrix
    fn evaluate_mass_matrix(&self, M: &mut VectorView);

    // Sets the local element state and optionally adds the element's contribution to the global internal forces,
    // tangent stiffness, or tangent damping matrix.
    fn set_state_and_evaluate(&mut self, u: &PositionView, v: &VelocityView, q: Option<&mut VectorView>, K: Option<&mut MatrixView>, D: Option<&mut MatrixView>);

    // Computes the element's potential energy based on the current state
    fn potential_energy(&self) -> f64;

    // Computes the element's kinetic energy based on the current state
    fn kinetic_energy(&self) -> f64;
}

impl_downcast!(Element);