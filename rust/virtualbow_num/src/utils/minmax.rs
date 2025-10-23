use itertools::Itertools;

// Finds and returns the maximum value in a vector of floats as well as the index at which it occurs.
pub fn discrete_maximum_1d(values: &[f64]) -> (f64, usize) {
    values.iter()
        .enumerate()
        .map(|(i, v)| (*v, i))
        .max_by(|(a, _), (b, _)| a.partial_cmp(b).expect("Failed to compare floating point values"))
        .expect("Failed to find maximum")
}

// Finds and returns the minimum value in a vector of floats as well as the index at which it occurs.
pub fn discrete_minimum_1d(values: &[f64]) -> (f64, usize) {
    values.iter()
        .enumerate()
        .map(|(i, v)| (*v, i))
        .min_by(|(a, _), (b, _)| a.partial_cmp(b).expect("Failed to compare floating point values"))
        .expect("Failed to find minimum")
}

// Finds the maximum of a function of integers by comparing all possible input combinations
pub fn discrete_maximum_nd<F, const N: usize>(f: &mut F, size: [usize; N]) -> (f64, [usize; N])
    where F: FnMut(&[usize; N]) -> f64
{
    let meta = (0..N).map(|k| 0..size[k]);       // Meta-iterator that produces an iterator for each index dimension
    let indices = meta.multi_cartesian_product();      // Cartesian product, i.e. all possible combinations of indices

    let values = indices.map(|i_vector| {
        let i_array = i_vector.try_into().unwrap();    // TODO: Get rid of temporary Vecs somehow?
        (f(&i_array), i_array)
    });

    values.max_by(|(a, _), (b, _)| a.partial_cmp(b).expect("Failed to compare floating point values"))
          .expect("Failed to find maximum")
}

// Finds the minimum of a function of integers by comparing all possible input combinations
pub fn discrete_minimum_nd<F, const N: usize>(f: &mut F, size: [usize; N]) -> (f64, [usize; N])
    where F: FnMut(&[usize; N]) -> f64
{
    let meta = (0..N).map(|k| 0..size[k]);       // Meta-iterator that produces an iterator for each index dimension
    let indices = meta.multi_cartesian_product();      // Cartesian product, i.e. all possible combinations of indices

    let values = indices.map(|i_vector| {
        let i_array = i_vector.try_into().unwrap();    // TODO: Get rid of temporary Vecs somehow?
        (f(&i_array), i_array)
    });

    values.min_by(|(a, _), (b, _)| a.partial_cmp(b).expect("Failed to compare floating point values"))
          .expect("Failed to find minimum")
}

#[cfg(test)]
mod tests {
    use assert2::assert;
    use super::*;

    #[test]
    fn test_discrete_minimum_1d() {
        // Panic on empty input
        let result = std::panic::catch_unwind(|| discrete_minimum_1d(&[]));
        assert!(result.is_err());

        // Panic on non-finite input
        let result = std::panic::catch_unwind(|| discrete_minimum_1d(&[f64::NAN, f64::NAN]));
        assert!(result.is_err());

        // Minimum of vector with single element
        let result = discrete_minimum_1d(&[0.0]);
        assert!(result == (0.0, 0));

        // Minimum of vector with multiple elements
        let result = discrete_minimum_1d(&[-1.0, 1.0, -2.0]);
        assert!(result == (-2.0, 2));
    }

    #[test]
    fn test_discrete_maximum_1d() {
        // Panic on empty input
        let result = std::panic::catch_unwind(|| discrete_maximum_1d(&[]));
        assert!(result.is_err());

        // Panic on non-finite input
        let result = std::panic::catch_unwind(|| discrete_maximum_1d(&[f64::NAN, f64::NAN]));
        assert!(result.is_err());

        // Maximum of vector with single element
        let result = discrete_maximum_1d(&[0.0]);
        assert!(result == (0.0, 0));

        // Maximum of vector with multiple elements
        let result = discrete_maximum_1d(&[-1.0, 1.0, -2.0]);
        assert!(result == (1.0, 1));
    }

    #[test]
    fn test_discrete_maximum_nd() {
        let result = discrete_maximum_nd(&mut |i| (i[0] as f64)*(i[1] as f64), [10, 10]);
        assert!(result == (81.0, [9, 9]));

        let values = [[0.0, 0.1, 0.2], [0.3, 0.2, 0.1]];
        let mut function = |i: &[usize; 2]| values[i[0]][i[1]];
        let result = discrete_maximum_nd(&mut function, [2, 3]);
        assert!(result == (0.3, [1, 0]));
    }

    #[test]
    fn test_discrete_minimum_nd() {
        let result = discrete_minimum_nd(&mut |i| (i[0] as f64)*(i[1] as f64), [10, 10]);
        assert!(result == (0.0, [0, 0]));

        let values = [[-0.0, -0.1, -0.2], [-0.3, -0.2, -0.1]];
        let mut function = |i: &[usize; 2]| values[i[0]][i[1]];
        let result = discrete_minimum_nd(&mut function, [2, 3]);
        assert!(result == (-0.3, [1, 0]));
    }
}