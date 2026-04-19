use nalgebra::{matrix, SMatrix, SVector};
use crate::fem::elements::beam::geometry::CrossSection;

// Simple rectangular cross section for use in tests

pub struct Section {
    ρ: f64,
    E: f64,
    G: f64,
    w: Vec<f64>,
    h: Vec<f64>,
    y: Vec<f64>
}

impl Section {
    pub fn new(ρ: f64, E: f64, G: f64, w: &[f64], h: &[f64], y: &[f64]) -> Self {
        Self {
            ρ,
            E,
            G,
            w: w.to_vec(),
            h: h.to_vec(),
            y: y.to_vec()
        }
    }

    fn offset(&self, n: f64) -> f64 {
        Self::interpolate(&self.y, n)
    }

    // Cubic interpolation between equidistant values y1, y2, y3 and y4, parameter x in [0, 1]
    fn interpolate(y: &[f64], n: f64) -> f64 {
        if y.len() == 1 {
            return y[0]
        }

        if y.len() == 2 {
            return (1.0 - n)*y[0] + n*y[1]
        }

        if y.len() == 4 {
            let c0 = y[0];
            let c1 = (2.0*y[3] - 9.0*y[2] + 18.0*y[1] - 11.0*y[0])/2.0;
            let c2 = (-9.0*y[3] + 36.0*y[2] - 45.0*y[1] + 18.0*y[0])/2.0;
            let c3 = (9.0*y[3] - 27.0*y[2] + 27.0*y[1] - 9.0*y[0])/2.0;
            
            return c0 + c1*n + c2*n.powi(2) + c3*n.powi(3);
        }

        panic!("Invalid length");
    }
}

impl CrossSection for Section {
    fn stiffness(&self, n: f64) -> SMatrix<f64, 3, 3> {
        let w = self.width(n);
        let h = self.height(n);
        let y = self.offset(n);

        let A = w*h;
        let I = w*h.powi(3)/12.0 + A*y.powi(2);

        let EA = self.E*A;
        let GA = self.G*A;
        let EI = self.E*I;

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

        let A = w*h;
        let I = w*h.powi(3)/12.0 + A*y.powi(2);

        let ρA = self.ρ*A;
        let ρI = self.ρ*I;

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