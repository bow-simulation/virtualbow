use nalgebra::{matrix, SMatrix, SVector};
use crate::fem::elements::beam::geometry::CrossSection;

// Simple rectangular cross section for use in tests

pub struct Section {
    ρ: f64,
    E: f64,
    G: f64,
    w: [f64; 4],
    h: [f64; 4],
    y: [f64; 4]
}

impl Section {
    pub fn constant(ρ: f64, E: f64, G: f64, w: f64, h: f64, y: f64) -> Self {
        Self {
            ρ,
            E,
            G,
            w: [w; 4],
            h: [h; 4],
            y: [y; 4]
        }
    }

    pub fn linear(ρ: f64, E: f64, G: f64, w: [f64; 2], h: [f64; 2], y: [f64; 2]) -> Self {
        Self {
            ρ,
            E,
            G,
            w: [w[0], w[0] + 1.0/3.0*(w[1] - w[0]), w[0] + 2.0/3.0*(w[1] - w[0]), w[1]],
            h: [h[0], h[0] + 1.0/3.0*(h[1] - h[0]), h[0] + 2.0/3.0*(h[1] - h[0]), h[1]],
            y: [y[0], w[0] + 1.0/3.0*(y[1] - y[0]), y[0] + 2.0/3.0*(y[1] - y[0]), y[1]],
        }
    }

    pub fn cubic(ρ: f64, E: f64, G: f64, w: [f64; 4], h: [f64; 4], y: [f64; 4]) -> Self {
        Self {
            ρ,
            E,
            G,
            w,
            h,
            y
        }
    }

    fn offset(&self, n: f64) -> f64 {
        Self::interpolate(&self.y, n)
    }

    // Cubic interpolation between equidistant values y1, y2, y3 and y4, parameter x in [0, 1]
    fn interpolate(y: &[f64; 4], x: f64) -> f64 {
        let c0 = y[0];
        let c1 = (2.0* y[3] - 9.0* y[2] + 18.0*y[1] - 11.0*y[0])/2.0;
        let c2 = (-9.0*y[3] + 36.0*y[2] - 45.0*y[1] + 18.0*y[0])/2.0;
        let c3 = (9.0*y[3] - 27.0*y[2] + 27.0*y[1] - 9.0*y[0])/2.0;

        c0 + c1 * x + c2* x.powi(2) + c3* x.powi(3)
    }
}

impl CrossSection for Section {
    fn stiffness(&self, n: f64) -> SMatrix<f64, 3, 3> {
        let w = self.width(n);
        let h = self.height(n);
        let y = self.offset(n);

        let EA = self.E*w*h;
        let GA = self.G*w*h;
        let EI = self.E*w*h.powi(3)/12.0;

        matrix![
            EA, 0.0, -EA*y;
            0.0, GA, 0.0;
            -EA*y, 0.0, EI;
        ]
    }

    fn mass(&self, n: f64) -> SMatrix<f64, 3, 3> {
        let w = self.width(n);
        let h = self.height(n);
        let y = self.offset(n);

        let ρA = self.ρ*w*h;
        let ρI = self.ρ*w*h.powi(3)/12.0;

        matrix![
            ρA, 0.0, -ρA*y;
            0.0, ρA, 0.0;
            -ρA*y, 0.0, ρI;
        ]
    }

    fn width(&self, n: f64) -> f64 {
        Self::interpolate(&self.w, n)
    }

    fn height(&self, n: f64) -> f64 {
        Self::interpolate(&self.h, n)
    }

    fn strain_recovery(&self, _n: f64) -> Vec<SVector<f64, 3>> {
        todo!()
    }

    fn stress_recovery(&self, _n: f64) -> Vec<SVector<f64, 3>> {
        todo!()
    }
}