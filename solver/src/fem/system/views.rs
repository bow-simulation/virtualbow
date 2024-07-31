use crate::fem::system::dof::Dof;

use nalgebra::{DVector, DMatrix, SVector, SMatrix};

// Local read-only view into the global position vector as defined by a list of dofs
pub struct PositionView<'a> {
    vector: &'a DVector<f64>,
    dofs: &'a [Dof]
}

impl<'a> PositionView<'a> {
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
            Dof::Fixed{offset} => offset,
            Dof::Free{index, offset, scale} => offset + scale*vector[index]
        }
    }
}

// Local read-only view into the global velocity vector as defined by a list of dofs
// Only difference is that the offset disappears from deriving the position
pub struct VelocityView<'a> {
    vector: &'a DVector<f64>,
    dofs: &'a [Dof]
}

impl<'a> VelocityView<'a> {
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
            Dof::Fixed{offset: _} => 0.0,
            Dof::Free{index, offset: _, scale} => scale*vector[index]
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
            vector: vector,
            dofs: dofs
        }
    }

    // Add the local contributions of all dofs as a fixed size vector
    // TODO: Assert against wrong dimension N?
    pub fn add<const N: usize>(&mut self, rhs: SVector<f64, N>) {
        for (row, value) in rhs.iter().enumerate() {
            match self.dofs[row] {
                Dof::Fixed{offset: _} => {
                    // Do nothing since the local value has no link to the global vector
                },
                Dof::Free{index, offset: _, scale} => {
                    self.vector[index] += scale*value;
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
                if let Dof::Free{index: i, offset: _, scale: si} = self.dofs[row] {
                    if let Dof::Free{index: j, offset: _, scale: sj} = self.dofs[col] {
                        self.matrix[(i, j)] += si*sj*rhs[(row, col)];
                    }
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::{DVector, DMatrix, SVector, SMatrix};
    use crate::fem::system::dof::Dof;
    use super::*;

    #[test]
    fn test_views() {
        let dof1 = Dof::Fixed { offset: 1.0 };
        let dof2 = Dof::Free { index: 2, offset: 1.0, scale: 1.5 };
        let dof3 = Dof::Free { index: 4, offset: 1.0, scale: 1.5 };
        let dof4 = Dof::Free { index: 6, offset: 1.0, scale: 1.5 };
        let dofs = &[dof1, dof2, dof3, dof4];

        {
            let vector = DVector::<f64>::from_column_slice(&[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0]);
            let view = PositionView::new(&vector, dofs);

            assert_eq!(view.get::<4>(), SVector::<f64, 4>::new(1.0, 5.5, 8.5, 11.5));
        }


        {
            let vector = DVector::<f64>::from_column_slice(&[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0]);
            let view = VelocityView::new(&vector, dofs);

            assert_eq!(view.get::<4>(), SVector::<f64, 4>::new(0.0, 4.5, 7.5, 10.5));
        }

        {
            let mut vector = DVector::<f64>::zeros(8);
            let mut view = VectorView::new(&mut vector, dofs);
            view.add(SVector::<f64, 4>::new(1.0, 2.0, 3.0, 4.0));

            assert_eq!(vector, DVector::<f64>::from_column_slice(&[0.0, 0.0, 3.0, 0.0, 4.5, 0.0, 6.0, 0.0]));
        }

        {
            let mut matrix = DMatrix::<f64>::zeros(8, 8);
            let mut view = MatrixView::new(&mut matrix, dofs);
            view.add(&SMatrix::<f64, 4, 4>::from_column_slice(&[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0]));

            assert_eq!(matrix[(2, 2)], 13.50);
            assert_eq!(matrix[(2, 4)], 22.50);
            assert_eq!(matrix[(2, 6)], 31.50);
            
            assert_eq!(matrix[(4, 2)], 15.75);
            assert_eq!(matrix[(4, 4)], 24.75);
            assert_eq!(matrix[(4, 6)], 33.75);

            assert_eq!(matrix[(6, 2)], 18.00);
            assert_eq!(matrix[(6, 4)], 27.00);
            assert_eq!(matrix[(6, 6)], 36.00);
        }
    }
}