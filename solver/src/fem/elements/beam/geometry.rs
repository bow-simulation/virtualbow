use nalgebra::{SMatrix, SVector, vector};

// This module contains common traits for describing a beam's geometry

// Planar curve, parameterized over arc length s
pub trait PlanarCurve {
    // Arc length at the start of the curve
    fn s_start(&self) -> f64;

    // Arc length at the end of the curve
    fn s_end(&self) -> f64;

    // Position vector [x(s), y(s)]
    fn position(&self, s: f64) -> SVector<f64, 2>;

    // Angle between curve tangent and the x axis
    fn angle(&self, s: f64) -> f64;

    // Curvature, first derivative of the tangent angle
    fn curvature(&self, s: f64) -> f64;

    // Arc length of the curve from start to end
    fn length(&self) -> f64 {
        self.s_end() - self.s_start()
    }

    // Position and angle [x(s), y(s), φ(s)]
    fn point(&self, s: f64) -> SVector<f64, 3> {
        let r = self.position(s);
        let φ = self.angle(s);
        vector![
            r[0],
            r[1],
            φ
        ]
    }
}

// Cross section properties, parameterized over arc length s
pub trait CrossSection {
    // Linear density, i.e. mass per unit length
    fn ρA(&self, s: f64) -> f64;

    // Rotary inertia per unit length
    fn rhoI(&self, s: f64) -> f64;

    // Full cross section stiffness matrix that describes the relation
    // (epilon, kappa, gamma) -> (normal force, bending moment, shear force)
    fn C(&self, s: f64) -> SMatrix<f64, 3, 3>;

    // Total width
    fn width(&self, s: f64) -> f64;

    // Total height of all layers
    fn height(&self, s: f64) -> f64;

    // Stress evaluation constants
    // TODO: Better name
    //fn stress(&self, s: f64) -> DMatrix<f64>;

    // Variation of strain over the cross section height
    fn strain(&self, epsilon: f64, kappa: f64, y: f64) -> f64 {
        epsilon - kappa*y
    }
}