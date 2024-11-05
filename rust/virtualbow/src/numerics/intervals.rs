use std::cmp::Ordering;

// Utilities for computations with intervals with inclusive/exclusive bounds.
// I can't believe I've written this just for checking the validity of the layer height inputs.

#[derive(Copy, Clone, Debug)]
pub enum Bound {
    Inclusive(f64),
    Exclusive(f64)
}

impl Bound {
    pub fn value(&self) -> f64 {
        match self {
            Bound::Inclusive(x) => *x,
            Bound::Exclusive(x) => *x,
        }
    }
}

impl PartialEq<Self> for Bound {
    // Two bounds are equal if they are of the same type and have the same value
    fn eq(&self, other: &Self) -> bool {
        if let (Bound::Inclusive(a), Bound::Inclusive(b)) = (self, other) {
            return a == b;
        }
        if let (Bound::Exclusive(a), Bound::Exclusive(b)) = (self, other) {
            return a == b;
        }
        return false;
    }
}

impl PartialOrd<Self> for Bound {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        // Compare values first, if not possible return None.
        let result = self.value().partial_cmp(&other.value())?;

        // If the values are not equal, return their ordering
        if result != Ordering::Equal {
            return Some(result);
        }

        // If the values are equal, the type of the bound decides
        match self {
            Bound::Inclusive(_) => match other {
                Bound::Inclusive(_) => Some(Ordering::Equal),
                Bound::Exclusive(_) => Some(Ordering::Less)
            }
            Bound::Exclusive(_) => match other {
                Bound::Inclusive(_) => Some(Ordering::Greater),
                Bound::Exclusive(_) => Some(Ordering::Equal)
            }
        }
    }
}

#[derive(PartialEq, Copy, Clone, Debug)]
pub struct Interval {
    pub lower: Bound,
    pub upper: Bound
}

impl Interval {
    pub fn inclusive(lower: f64, upper: f64) -> Self {
        Self {
            lower: Bound::Inclusive(lower),
            upper: Bound::Inclusive(upper),
        }
    }

    pub fn exclusive(lower: f64, upper: f64) -> Self {
        Self {
            lower: Bound::Exclusive(lower),
            upper: Bound::Exclusive(upper),
        }
    }

    pub fn intersects(&self, other: &Interval) -> bool {
        self.lower <= other.upper && other.lower <= self.upper
    }

    // Given a list of intervals, this function returns the leftmost partial interval of their union set.
    // This is the interval that they "cover" when starting from the left without creating a "hole".
    pub fn left_union(mut intervals: Vec<Interval>) -> Interval {
        assert!(intervals.len() > 0);

        // Sort intervals by their lower bounds and use leftmost interval as the starting value for the result
        intervals.sort_by(|a, b| a.lower.partial_cmp(&b.lower).expect("Failed to sort intervals"));
        let mut result = intervals[0];

        // Iterate over remaining intervals
        // If the new interval doesn't intersect the current result, union ends there.
        // Otherwise extend the current result by the upper bound of the new interval.
        for next in intervals.iter().skip(1) {
            if !next.intersects(&result) {
                break;
            }
            else if next.upper > result.upper {
                result.upper = next.upper;
            }
        }

        result
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_bound_equality() {
        assert_eq!(Bound::Inclusive(0.0), Bound::Inclusive(0.0));    // Equal: Same type and same value
        assert_eq!(Bound::Exclusive(0.0), Bound::Exclusive(0.0));    // Equal: Same type and same value

        assert_ne!(Bound::Inclusive(0.0), Bound::Inclusive(1.0));    // Not equal: Same type and different value
        assert_ne!(Bound::Exclusive(0.0), Bound::Exclusive(1.0));    // Not equal: Same type and different value

        assert_ne!(Bound::Inclusive(0.0), Bound::Exclusive(0.0));    // Not equal: Different type and same value
        assert_ne!(Bound::Inclusive(0.0), Bound::Exclusive(1.0));    // Not equal: Different type and different value
    }

    #[test]
    fn test_bound_ordering() {
        assert!(Bound::Inclusive(0.0) < Bound::Inclusive(1.0));  // Same type, different values
        assert!(Bound::Inclusive(1.0) > Bound::Inclusive(0.0));  // Same type, different values

        assert!(Bound::Exclusive(0.0) < Bound::Exclusive(1.0));  // Same type, different values
        assert!(Bound::Exclusive(1.0) > Bound::Exclusive(0.0));  // Same type, different values

        assert!(Bound::Inclusive(0.0) < Bound::Exclusive(0.0));  // Same value, different types
        assert!(Bound::Exclusive(0.0) > Bound::Inclusive(0.0));  // Same value, different types
    }

    #[test]
    fn test_interval_union() {
        let intervals = vec![
            Interval { lower: Bound::Inclusive(0.5), upper: Bound::Inclusive(1.5) },
            Interval { lower: Bound::Inclusive(2.0), upper: Bound::Inclusive(3.0) },
            Interval { lower: Bound::Inclusive(0.0), upper: Bound::Inclusive(1.0) },
        ];

        let result = Interval::left_union(intervals);
        assert_eq!(result, Interval { lower: Bound::Inclusive(0.0), upper: Bound::Inclusive(1.5) });
    }
}