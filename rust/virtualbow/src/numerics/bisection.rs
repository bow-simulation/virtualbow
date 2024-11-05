use std::cmp::Ordering;
use num::Float;

// Return the index where x should be inserted into a, assuming a is sorted.
// The return value i is such that all e in a[..i] have e <= x, and all e in a[i..] have e > x.
// If a contains x, a.insert(i, x) will insert just after the rightmost occurrence of x.
//
// Inspired by the bisection crate [1], which unfortunately does not work with floats due to requiring Ord [2].
// [1] https://crates.io/crates/bisection
// [2] https://github.com/SteadBytes/bisection/pull/1
pub fn bisect_right<T: Float>(a: &[T], x: T) -> usize {
    bisect_right_by(a, |p| p.partial_cmp(&x).expect("Failed to compare floating point values"))
}

// Return the index where x should be inserted in a, assuming a is sorted, according to a comparator function.
// The comparator function should implement an order consistent with the sort order of the underlying slice,
// returning an order code that indicates whethers its argument is Less, Equal or Greater that the desired target.
pub fn bisect_right_by<T, F>(a: &[T], mut f: F) -> usize
    where F: FnMut(&T) -> Ordering,
{
    assert!(a.len() >= 2);

    let mut lower = 0;
    let mut upper = a.len() - 1;

    while upper - lower > 1 {
        let middle = (lower + upper)/2;
        if f(&a[middle]) == Ordering::Greater {
            upper = middle;
        }
        else {
            lower = middle;
        }
    }

    lower
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_bisection() {
        let x = &[0.0, 1.0, 2.0, 3.0];

        assert_eq!(bisect_right(x, -1.0), 0);
        assert_eq!(bisect_right(x, 0.0), 0);
        assert_eq!(bisect_right(x, 0.5), 0);
        assert_eq!(bisect_right(x, 1.0), 1);
        assert_eq!(bisect_right(x, 1.5), 1);
        assert_eq!(bisect_right(x, 2.0), 2);
        assert_eq!(bisect_right(x, 2.5), 2);
        assert_eq!(bisect_right(x, 3.0), 2);
        assert_eq!(bisect_right(x, 3.5), 2);
    }
}