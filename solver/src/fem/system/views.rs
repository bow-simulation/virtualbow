use crate::fem::system::dof::Dof;

use nalgebra::{DVector, DMatrix, SVector, SMatrix};

// Some shorthands for more clarity
pub type PositionView<'a> = AffineView<'a>;
pub type VelocityView<'a> = LinearView<'a>;
pub type AccelerationView<'a> = LinearView<'a>;
pub type ForceView<'a> = LinearView<'a>;

// Local read-only view into a global vector as defined by a list of dofs, including offsets
pub struct AffineView<'a> {
    vector: &'a DVector<f64>,
    dofs: &'a [Dof]
}

impl<'a> AffineView<'a> {
    // Create a new view that references the given vector and dofs
    pub fn new(vector: &'a DVector<f64>, dofs: &'a [Dof]) -> Self {
        Self {
            vector: vector,
            dofs: dofs
        }
    }

    // Get the local position associated with the dof of given index
    pub fn at(&self, index: usize) -> f64 {
        Self::transform(self.vector, self.dofs[index])
    }

    // Get the local positions of all dofs as a fixed size vector
    // TODO: Assert against wrong dimension N?
    pub fn get<const N: usize>(&self) -> SVector<f64, N> {
        SVector::<f64, N>::from_fn(|row, _| {
            Self::transform(self.vector, self.dofs[row])
        })
    }

    // Transform from vector and dof to scalar value. Static function to be used independently.
    pub fn transform(vector: &DVector<f64>, dof: Dof) -> f64 {
        match dof {
            Dof::Fixed(u) => u,
            Dof::Free(i) => vector[i]
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

    // Get the local position associated with the dof of given index
    pub fn at(&self, index: usize) -> f64 {
        Self::transform(self.vector, self.dofs[index])
    }

    // Get the local positions of all dofs as a fixed size vector
    // TODO: Assert against wrong dimension N?
    pub fn get<const N: usize>(&self) -> SVector<f64, N> {
        SVector::<f64, N>::from_fn(|row, _| {
            Self::transform(self.vector, self.dofs[row])
        })
    }

    // Transform from vector and dof to scalar value. Static function to be used independently.
    pub fn transform(vector: &DVector<f64>, dof: Dof) -> f64 {
        match dof {
            Dof::Fixed(_) => 0.0,
            Dof::Free(i) => vector[i]
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

    // Add the local contributions of all dofs as a fixed size vector
    // TODO: Assert against wrong dimension N?
    pub fn add<const N: usize>(&mut self, rhs: SVector<f64, N>) {
        for (row, value) in rhs.iter().enumerate() {
            match self.dofs[row] {
                Dof::Fixed(_) => {
                    // Do nothing since the local value has no link to the global vector
                },
                Dof::Free(i) => {
                    self.vector[i] += value;
                }
            }
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
            matrix: matrix,
            dofs: dofs
        }
    }

    // Add the local contributions of all dofs as a fixed size matrix
    // TODO: Assert against wrong dimension N?
    pub fn add<const N: usize>(&mut self, rhs: &SMatrix<f64, N, N>) {
        for row in 0..N {
            for col in 0..N {
                if let Dof::Free(i) = self.dofs[row] {
                    if let Dof::Free(j) = self.dofs[col] {
                        self.matrix[(i, j)] += rhs[(row, col)];
                    }
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::{DVector, DMatrix, SVector, SMatrix, vector, dvector, matrix, dmatrix};
    use crate::fem::system::dof::Dof;
    use super::*;

    #[test]
    fn test_views() {
        let dof1 = Dof::Fixed(1.0);
        let dof2 = Dof::Free(2);
        let dof3 = Dof::Free(4);
        let dof4 = Dof::Free(6);
        let dofs = &[dof1, dof2, dof3, dof4];

        {
            let vector = dvector![1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0];
            let view = PositionView::new(&vector, dofs);

            assert_eq!(view.get::<4>(), vector![1.0, 3.0, 5.0, 7.0]);
        }


        {
            let vector = dvector![1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0];
            let view = VelocityView::new(&vector, dofs);

            assert_eq!(view.get::<4>(), vector![0.0, 3.0, 5.0, 7.0]);
        }

        {
            let mut vector = DVector::<f64>::zeros(8);
            let mut view = VectorView::new(&mut vector, dofs);
            view.add(vector![1.0, 2.0, 3.0, 4.0]);

            assert_eq!(vector, dvector![0.0, 0.0, 2.0, 0.0, 3.0, 0.0, 4.0, 0.0]);
        }

        {
            let mut matrix = DMatrix::<f64>::zeros(8, 8);
            let mut view = MatrixView::new(&mut matrix, dofs);
            view.add(&matrix![1.0, 2.0, 3.0, 4.0; 5.0, 6.0, 7.0, 8.0; 9.0, 10.0, 11.0, 12.0; 13.0, 14.0, 15.0, 16.0]);

            assert_eq!(matrix, dmatrix![
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