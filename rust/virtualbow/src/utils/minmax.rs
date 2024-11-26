use itertools::Itertools;

// Finds and returns the maximum value in a vector of floats as well as the index at which it occurs.
pub fn discrete_maximum_1d(values: &Vec<f64>) -> (f64, usize) {
    values.iter()
        .enumerate()
        .map(|(i, v)| (*v, i))
        .max_by(|(a, _), (b, _)| a.partial_cmp(b).expect("Failed to compare floating point values"))
        .expect("Failed to find maximum")
}

// Finds and returns the maximum value in a nested vector of floats as well as the index pair at which it occurs.
pub fn discrete_maximum_2d(values: &Vec<Vec<f64>>) -> (f64, usize, usize) {
    values.iter()
        .enumerate()
        .map(|(i, v)| {
            let (a, j) = discrete_maximum_1d(&v);
            (a, i, j)
        })
        .max_by(|(a, _, _), (b, _, _)| a.partial_cmp(b).expect("Failed to compare floating point values"))
        .expect("Failed to find maximum")
}

/*
pub fn discrete_maximum<F, const N: usize>(f: F, imax: [usize; N]) -> (f64, [usize; N])
    where F: FnMut(&[usize; N]) -> f64
{
    if N == 1 {
        (0..imax[0]).map(|i| f(&[i]))

    }

    unimplemented!();

    /*
    let meta = (0..N).map(|k| 0..imax[k]);    // Meta-iterator that produces an iterator for each dimension
    let iter = meta.multi_cartesian_product();      // Cartesian product, i.e. all possible combinations of indices

    let max = iter.map(|indices| f(&indices))

    for v in iter {
        println!("v = {:?}", v);
    }
    */

    (0.0, [0; N])
}
*/

#[cfg(test)]
mod tests {
    use super::*;

    /*
    #[test]
    fn test_discrete_maximum() {
        let result = discrete_maximum(|i| (i[0] as f64)*(i[1] as f64), [10, 10]);
        println!("result = {:?}", result);
    }
    */

    #[test]
    fn test_discrete_maximum_1d() {
        // Panic on empty input
        let result = std::panic::catch_unwind(|| discrete_maximum_1d(&vec![]));
        assert!(result.is_err());

        // Panic on non-finite input
        let result = std::panic::catch_unwind(|| discrete_maximum_1d(&vec![f64::NAN, f64::NAN]));
        assert!(result.is_err());

        // Maximum of vector with single element
        let result = discrete_maximum_1d(&vec![0.0]);
        assert_eq!(result, (0.0, 0));

        // Maximum of vector with multiple elements
        let result = discrete_maximum_1d(&vec![-1.0, 1.0, -2.0]);
        assert_eq!(result, (1.0, 1));
    }

    #[test]
    fn test_discrete_maximum_2d() {
        // Panic on empty input
        let result = std::panic::catch_unwind(|| discrete_maximum_2d(&vec![]));
        assert!(result.is_err());

        // Panic on non-finite input
        let result = std::panic::catch_unwind(|| discrete_maximum_2d(&vec![vec![f64::NAN, f64::NAN]]));
        assert!(result.is_err());

        // Maximum of vector with single element
        let result = discrete_maximum_2d(&vec![vec![0.0]]);
        assert_eq!(result, (0.0, 0, 0));

        // Maximum of vector with multiple elements
        let result = discrete_maximum_2d(&vec![vec![-1.0, 1.0, -2.0], vec![-2.0, 2.0, -4.0], vec![-1.0, 1.0, -2.0]]);
        assert_eq!(result, (2.0, 1, 1));
    }
}