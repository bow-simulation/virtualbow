use nalgebra::{SMatrix, SVector};

// This module contains common traits for describing a beam's geometry

// Planar curve, parameterized over arc length s
pub trait PlanarCurve {
    // Total arc length of the curve
    fn length(&self) -> f64;

    // Position vector [x(s), y(s)] over arc length
    fn position(&self, s: f64) -> SVector<f64, 2>;

    // Angle between curve tangent and the x-axis
    fn angle(&self, s: f64) -> f64;

    // Curvature, first derivative of the tangent angle
    fn curvature(&self, s: f64) -> f64;

    // Converts the given arc length to a normalized position from 0 to 1
    fn normalize(&self, s: f64) -> f64 {
        s/self.length()
    }

    // Position and angle [x(s), y(s), φ(s)]
    // TODO: Better name for this?
    // TODO: Remove position, angle and curvature for a single evaluation method?
    fn point(&self, s: f64) -> [f64; 3] {
        let r = self.position(s);
        let φ = self.angle(s);
        [
            r[0],
            r[1],
            φ
        ]
    }
}

// Cross-section properties, parameterized over the normalized position p from 0 to 1
pub trait CrossSection {
    // Full cross-section stiffness matrix that describes the relation
    // (epsilon, kappa, gamma) -> (normal force, bending moment, shear force)
    fn stiffness(&self, n: f64) -> SMatrix<f64, 3, 3>;

    // Full cross-section mass matrix
    fn mass(&self, n: f64) -> SMatrix<f64, 3, 3>;

    // Total width
    fn width(&self, n: f64) -> f64;

    // Total height
    fn height(&self, n: f64) -> f64;

    // Returns the strain recovery matrices for the cross-section at relative position n and for implementation-specific points of interest.
    // When multiplied with the strain vector [epsilon, gamma, kappa], each matrix produces the normal strain at that point.
    fn strain_recovery(&self, n: f64) -> Vec<SVector<f64, 3>>;

    // Returns the stress recovery matrices for the cross-section at relative position n and for implementation-specific points of interest.
    // When multiplied with the strain vector [epsilon, gamma, kappa], each matrix produces the normal stress at that point.
    fn stress_recovery(&self, n: f64) -> Vec<SVector<f64, 3>>;
}