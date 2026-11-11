use super::views::{MatrixView, PositionView, VelocityView, VectorView};
use downcast_rs::{Downcast, impl_downcast};

pub trait Element: Downcast {
    // Adds the element's contribution to the global mass matrix
    fn evaluate_mass_matrix(&self, M: &mut VectorView);

    // Sets the local element state and optionally adds the element's contribution to the global internal forces,
    // tangent stiffness, or tangent damping matrix.
    fn update_and_evaluate(&mut self, u: &PositionView, v: &VelocityView, q: Option<&mut VectorView>, K: Option<&mut MatrixView>, D: Option<&mut MatrixView>);

    // Adds the derivative of the element's internal forces with respect to its parameter, evaluated at
    // the state of the last update. The parameter is the one scalar property of an element that a solver
    // may treat as an unknown, like the unstressed length of a string. Elements that don't have one leave
    // the result untouched, which is what the default implementation does.
    fn evaluate_parameter_derivative(&self, _dqdp: &mut VectorView) {}

    // Computes the element's potential energy based on the current state
    fn potential_energy(&self) -> f64;

    // Computes the element's kinetic energy based on the current state
    fn kinetic_energy(&self) -> f64;

    // Computes the current power of the dissipative forces
    fn dissipative_power(&self) -> f64;
}

impl_downcast!(Element);