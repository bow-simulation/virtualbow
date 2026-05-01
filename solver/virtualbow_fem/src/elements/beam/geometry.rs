use nalgebra::{SMatrix, SVector, matrix, vector};

// This module contains common traits for describing a beam's geometry

// Planar curve, parameterized over arc length s
pub trait PlanarCurve {
    // Arc length at the start of the curve
    fn length_start(&self) -> f64;

    // Arc length at the end of the curve
    fn length_end(&self) -> f64;

    // Position vector [x(s), y(s)]
    fn position(&self, s: f64) -> SVector<f64, 2>;

    // Angle between curve tangent and the x axis
    fn angle(&self, s: f64) -> f64;

    // Curvature, first derivative of the tangent angle
    fn curvature(&self, s: f64) -> f64;

    // Arc length of the curve from start to end
    fn length(&self) -> f64 {
        self.length_end() - self.length_start()
    }

    // Converts the given arc length to a normalized position from 0 to 1
    fn normalize(&self, s: f64) -> f64 {
        (s - self.length_start())/self.length()
    }

    // Position and angle [x(s), y(s), φ(s)]
    // TODO: Better name for this?
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

// Cross section properties, parameterized over the normalized position p from 0 to 1
pub trait CrossSection {
    // Full cross section stiffness matrix that describes the relation
    // (epsilon, kappa, gamma) -> (normal force, bending moment, shear force)
    fn stiffness(&self, n: f64) -> SMatrix<f64, 3, 3>;

    // Full cross section mass matrix
    fn mass(&self, n: f64) -> SMatrix<f64, 3, 3>;

    // Total width
    fn width(&self, n: f64) -> f64;

    // Total height
    fn height(&self, n: f64) -> f64;

    // Returns the strain recovery matrices for the cross section at relative position n and for implementation-specific points of interest.
    // When multiplied with the strain vector [epsilon, gamma, kappa], each matrix matrix produces the normal strain at that point.
    fn strain_recovery(&self, n: f64) -> Vec<SVector<f64, 3>>;

    // Returns the stress recovery matrices for the cross section at relative position n and for implementation-specific points of interest.
    // When multiplied with the strain vector [epsilon, gamma, kappa], each matrix matrix produces the normal stress at that point.
    fn stress_recovery(&self, n: f64) -> Vec<SVector<f64, 3>>;
}

// Implementation of a linearly varying rectangular cross section for use in tests
pub struct RectangularSection {
    pub w0: f64,
    pub h0: f64,
    pub w1: f64,
    pub h1: f64,
    pub ρ: f64,
    pub E: f64,
    pub G: f64,
}

impl CrossSection for RectangularSection {
    fn stiffness(&self, n: f64) -> SMatrix<f64, 3, 3> {
        let w = self.width(n);
        let h = self.height(n);

        let EA = self.E*w*h;
        let GA = self.G*w*h;
        let EI = self.E*w*h.powi(3)/12.0;

        matrix![
            EA, 0.0, 0.0;
            0.0, GA, 0.0;
            0.0, 0.0, EI;
        ]
    }

    fn mass(&self, n: f64) -> SMatrix<f64, 3, 3> {
        let w = self.width(n);
        let h = self.height(n);

        let ρA = self.ρ*w*h;
        let ρI = self.ρ*w*h.powi(3)/12.0;

        matrix![
            ρA, 0.0, 0.0;
            0.0, ρA, 0.0;
            0.0, 0.0, ρI;
        ]
    }

    fn width(&self, p: f64) -> f64 {
        self.w0 + p*(self.w1 - self.w0)
    }

    fn height(&self, p: f64) -> f64 {
        self.h0 + p*(self.h1 - self.h0)
    }

    fn strain_recovery(&self, _p: f64) -> Vec<SVector<f64, 3>> {
        todo!();
    }

    fn stress_recovery(&self, _p: f64) -> Vec<SVector<f64, 3>> {
        todo!();
    }
}

// Implementation of a straight line curve for use in tests
pub struct LineCurve {
    pub x: f64,
    pub y: f64,
    pub φ: f64,
    pub l: f64,
}

impl PlanarCurve for LineCurve {
    fn length_start(&self) -> f64 {
        0.0
    }

    fn length_end(&self) -> f64 {
        self.l
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        vector![
            self.x + s*f64::cos(self.φ),
            self.y + s*f64::sin(self.φ),
        ]
    }

    fn angle(&self, _s: f64) -> f64 {
        self.φ
    }

    fn curvature(&self, _s: f64) -> f64 {
        0.0
    }
}

// Implementation of a circular arc curve for use in tests
pub struct ArcCurve {
    pub x: f64,
    pub y: f64,
    pub φ: f64,
    pub l: f64,
    pub r: f64,
}

impl PlanarCurve for ArcCurve {
    fn length_start(&self) -> f64 {
        0.0
    }

    fn length_end(&self) -> f64 {
        self.l
    }

    fn position(&self, s: f64) -> SVector<f64, 2> {
        vector![
                self.x + self.r*(f64::sin(s/self.r + self.φ) - f64::sin(self.φ)),
                self.y + self.r*(f64::cos(self.φ) - f64::cos(s/self.r + self.φ))
            ]
    }

    fn angle(&self, s: f64) -> f64 {
        self.φ + s/self.r
    }

    fn curvature(&self, _s: f64) -> f64 {
        1.0/self.r
    }
}