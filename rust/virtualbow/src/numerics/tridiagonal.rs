// Tridiagonal matrix data structure and solver, ported from https://github.com/ttk592/spline
#[derive(Debug)]
pub struct TDMatrix {
    a: Vec<f64>,	// Lower secondary diagonal
    b: Vec<f64>,	// Diagonal
    c: Vec<f64>	    // Upper secondary diagonal
}

impl TDMatrix {
    // Construct tridiagonal matrix of given size, initialized with zeros
    pub fn zeros(n: usize) -> Self {
        assert!(n > 0);
        Self {
            a: vec![0.0; n-1],
            b: vec![0.0; n],
            c: vec![0.0; n-1]
        }
    }

    // Matrix dimension, i.e. number of rows and columns
    pub fn size(&self) -> usize {
        self.b.len()
    }

    // Get entry at given row and column index
    pub fn get(&self, i: usize, j: usize) -> f64 {
        assert!(i < self.size() && j < self.size());
        match (i as i32) - (j as i32) {
            1 => self.a[j],
            0 => self.b[j],
            -1 => self.c[i],
            _ => 0.0
        }
    }

    // Set entry at given row and column index (must lie on one of the diagonals)
    pub fn set(&mut self, i: usize, j: usize, val: f64) {
        assert!(i < self.size() && j < self.size());
        match (i as i32) - (j as i32) {
            1 => self.a[j] = val,
            0 => self.b[j] = val,
            -1 => self.c[i] = val,
            _ => panic!("This entry is zero and cannot be set")
        }
    }

    // Solves a linear equation for the given right hand side in-place
    // https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
    pub fn solve(mut self, rhs: &mut Vec<f64>) {
        assert_eq!(rhs.len(), self.size());

        let n = self.size() - 1;

        self.c[0] /= self.b[0];
        rhs[0] /= self.b[0];

        for i in 1..n {
            let m = self.b[i] - self.a[i-1]*self.c[i-1];
            self.c[i] = self.c[i]/m;
            rhs[i] = (rhs[i] - self.a[i-1]*rhs[i-1])/m;
        }

        rhs[n] = (rhs[n] - self.a[n-1]*rhs[n-1])/(self.b[n] - self.a[n-1]*self.c[n-1]);

        for i in (1..n+1).rev() {
            rhs[i-1] -= self.c[i-1]*rhs[i];
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_tridiagonal_solve() {
        // Test case: Given the matrix and the right hand side, find the vector on the left.
        //
        // │ 2, 1, 0, 0 │   │ 1 │   │  4 │
        // │ 1, 2, 1, 0 │ * │ 2 │ = │  8 │
        // │ 0, 1, 2, 1 │   │ 3 │   │ 12 │
        // │ 0, 0, 1, 2 │   │ 4 │   │ 11 │

        let mut matrix = TDMatrix::zeros(4);

        matrix.set(0, 0, 2.0);
        matrix.set(1, 1, 2.0);
        matrix.set(2, 2, 2.0);
        matrix.set(3, 3, 2.0);

        matrix.set(0, 1, 1.0);
        matrix.set(1, 2, 1.0);
        matrix.set(2, 3, 1.0);

        matrix.set(1, 0, 1.0);
        matrix.set(2, 1, 1.0);
        matrix.set(3, 2, 1.0);

        let mut rhs = vec![4.0, 8.0, 12.0, 11.0];
        matrix.solve(&mut rhs);

        assert_abs_diff_eq!(rhs[0], 1.0, epsilon=1e-15);
        assert_abs_diff_eq!(rhs[1], 2.0, epsilon=1e-15);
        assert_abs_diff_eq!(rhs[2], 3.0, epsilon=1e-15);
        assert_abs_diff_eq!(rhs[3], 4.0, epsilon=1e-15);
    }
}