use crate::fem::system::dof::{Dof, DofType};

use nalgebra::{DVector, DMatrix, SVector, SMatrix};

// TODO: Common trait for AffineView and LinearView? Divided into read and write?

// Some shorthands for more clarity
pub type DisplacementView<'a> = LinearView<'a>;
pub type VelocityView<'a> = LinearView<'a>;
pub type AccelerationView<'a> = LinearView<'a>;
pub type ForceView<'a> = LinearView<'a>;

// Local read-only view into a global vector as defined by a list of dofs, including offsets
pub struct PositionView<'a> {
    x_locked: &'a DVector<f64>,
    x_active: &'a DVector<f64>,
    u_active: &'a DVector<f64>,
    dofs: &'a [Dof]
}

impl<'a> PositionView<'a> {
    // Create a new view that references the given vector and dofs
    pub fn new(x_locked: &'a DVector<f64>, x_active: &'a DVector<f64>, u_active: &'a DVector<f64>, dofs: &'a [Dof]) -> Self {
        Self {
            x_locked,
            x_active,
            u_active,
            dofs
        }
    }

    // Number of dofs
    #[allow(dead_code)]
    pub fn len(&self) -> usize {
        self.dofs.len()
    }

    // Get the local position associated with the dof of given index
    pub fn at(&self, index: usize) -> f64 {
        Self::transform(self.x_locked, self.x_active, self.u_active, self.dofs[index])
    }

    // Get the local positions of all dofs as a fixed size vector
    // TODO: Assert against wrong dimension N?
    pub fn get<const N: usize>(&self) -> SVector<f64, N> {
        SVector::<f64, N>::from_fn(|row, _| self.at(row))
    }

    // Transform from vector and dof to scalar value. Static function to be used independently.
    pub fn transform(x_locked: &DVector<f64>, x_active: &DVector<f64>, u_active: &DVector<f64>, dof: Dof) -> f64 {
        match dof.kind {
            DofType::Locked => {
                x_locked[dof.index]
            }
            DofType::Active => {
                x_active[dof.index] + u_active[dof.index]
            }
        }
    }
}

// Local read-only view into a global vector as defined by a list of dofs, disregarding offsets
pub struct LinearView<'a> {
    vector: &'a DVector<f64>,
    dofs: &'a [Dof]
}

impl<'a> LinearView<'a> {
    // Create a new view that references the given vector and dofs
    pub fn new(vector: &'a DVector<f64>, dofs: &'a [Dof]) -> Self {
        Self {
            vector,
            dofs
        }
    }

    // Number of dofs
    #[allow(dead_code)]
    pub fn len(&self) -> usize {
        self.dofs.len()
    }

    // Get the local position associated with the dof of given index
    pub fn at(&self, index: usize) -> f64 {
        Self::transform(self.vector, self.dofs[index])
    }

    // Get the local positions of all dofs as a fixed size vector
    // TODO: Assert against wrong dimension N?
    pub fn get<const N: usize>(&self) -> SVector<f64, N> {
        SVector::<f64, N>::from_fn(|row, _| self.at(row))
    }

    // Transform from vector and dof to scalar value. Static function to be used independently.
    pub fn transform(vector: &DVector<f64>, dof: Dof) -> f64 {
        match dof.kind {
            DofType::Locked => {
                0.0
            }
            DofType::Active => {
                vector[dof.index]
            }
        }
    }
}

// Local write-only view into a global vector (used for elastic forces) as defined by a list of dofs
pub struct VectorView<'a> {
    vector: &'a mut DVector<f64>,
    dofs: &'a [Dof]
}

impl<'a> VectorView<'a> {
    // Create a new view that references the given vector and dofs
    pub fn new(vector: &'a mut DVector<f64>, dofs: &'a [Dof]) -> Self {
        Self {
            vector,
            dofs
        }
    }

    // Number of dofs
    #[allow(dead_code)]
    pub fn len(&self) -> usize {
        self.dofs.len()
    }

    pub fn add(&mut self, row: usize, value: f64) {
        match self.dofs[row].kind {
            DofType::Locked => {
                // Do nothing since the local value has no link to the global vector
            },
            DofType::Active => {
                self.vector[self.dofs[row].index] += value;
            }
        }
    }

    // Add the local contributions of all dofs as a fixed size vector
    // TODO: Assert against wrong dimension N?
    // TODO: Replace with implementation of operator +=?
    pub fn add_vec<const N: usize>(&mut self, rhs: SVector<f64, N>) {
        for (row, &value) in rhs.iter().enumerate() {
            self.add(row, value);
        }
    }
}

// Local write-only view into a global matrix (mass, damping, stiffness) as defined by a list of dofs
pub struct MatrixView<'a> {
    matrix: &'a mut DMatrix<f64>,
    dofs: &'a [Dof]
}

impl<'a> MatrixView<'a> {
    // Create a new view that references the given vector and dofs
    pub fn new(matrix: &'a mut DMatrix<f64>, dofs: &'a [Dof]) -> Self {
        Self {
            matrix,
            dofs
        }
    }

    // Number of dofs
    #[allow(dead_code)]
    pub fn len(&self) -> usize {
        self.dofs.len()
    }

    pub fn add(&mut self, row: usize, col: usize, value: f64) {
        if self.dofs[row].is_active() && self.dofs[col].is_active() {
            self.matrix[(self.dofs[row].index, self.dofs[col].index)] += value;
        }
    }

    // Add the local contributions of all dofs as a fixed size matrix
    // TODO: Assert against wrong dimension N?
    // TODO: Replace with implementation of operator +=?
    pub fn add_mat<const N: usize>(&mut self, rhs: &SMatrix<f64, N, N>) {
        for row in 0..N {
            for col in 0..N {
                self.add(row, col, rhs[(row, col)]);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::{DVector, DMatrix, vector, dvector, matrix, dmatrix};
    use crate::fem::system::dof::Dof;
    use assert2::assert;
    use super::*;

    #[test]
    fn test_views() {
        let dof1 = Dof { kind: DofType::Locked, index: 0};
        let dof2 = Dof { kind: DofType::Active, index: 2};
        let dof3 = Dof { kind: DofType::Active, index: 4};
        let dof4 = Dof { kind: DofType::Active, index: 6};
        let dofs = &[dof1, dof2, dof3, dof4];

        {
            let pos_locked = dvector![1.0];
            let pos_active = dvector![0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8];
            let displacements = dvector![1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0];
            let view = PositionView::new(&pos_locked, &pos_active, &displacements, dofs);

            assert!(view.get::<4>() == vector![1.0, 3.3, 5.5, 7.7]);
        }


        {
            let vector = dvector![1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0];
            let view = VelocityView::new(&vector, dofs);

            assert!(view.get::<4>() == vector![0.0, 3.0, 5.0, 7.0]);
        }

        {
            let mut vector = DVector::<f64>::zeros(8);
            let mut view = VectorView::new(&mut vector, dofs);
            view.add_vec(vector![1.0, 2.0, 3.0, 4.0]);

            assert!(vector == dvector![0.0, 0.0, 2.0, 0.0, 3.0, 0.0, 4.0, 0.0]);
        }

        {
            let mut matrix = DMatrix::<f64>::zeros(8, 8);
            let mut view = MatrixView::new(&mut matrix, dofs);
            view.add_mat(&matrix![1.0, 2.0, 3.0, 4.0; 5.0, 6.0, 7.0, 8.0; 9.0, 10.0, 11.0, 12.0; 13.0, 14.0, 15.0, 16.0]);

            assert!(matrix == dmatrix![
                0.0, 0.0,  0.0, 0.0,  0.0, 0.0,  0.0, 0.0;
                0.0, 0.0,  0.0, 0.0,  0.0, 0.0,  0.0, 0.0;
                0.0, 0.0,  6.0, 0.0,  7.0, 0.0,  8.0, 0.0;
                0.0, 0.0,  0.0, 0.0,  0.0, 0.0,  0.0, 0.0;
                0.0, 0.0, 10.0, 0.0, 11.0, 0.0, 12.0, 0.0;
                0.0, 0.0,  0.0, 0.0,  0.0, 0.0,  0.0, 0.0;
                0.0, 0.0, 14.0, 0.0, 15.0, 0.0, 16.0, 0.0;
                0.0, 0.0,  0.0, 0.0,  0.0, 0.0,  0.0, 0.0;
            ]);
        }
    }
}