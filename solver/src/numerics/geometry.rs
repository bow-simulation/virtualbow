use nalgebra::SVector;

// Describes the orientation of a turn that three numbered points in a plane make.
#[derive(Debug, Copy, Clone, PartialEq)]
pub enum Orientation {
    LeftTurn,     // Left turn, i.e. counterclockwise
    RightTurn,    // Right turn, i.e. clockwise
    Collinear     // Points lie in a perfect line
}

impl Orientation {
    fn from_points(a: &SVector<f64, 2>, b: &SVector<f64, 2>, c: &SVector<f64, 2>) -> Self {
        let product = (b[0] - a[0])*(c[1] - a[1]) - (b[1] - a[1])*(c[0] - a[0]);

        if product > 0.0 {
            return Orientation::LeftTurn;
        }
        if product < 0.0 {
            return Orientation::RightTurn;
        }
        return Orientation::Collinear;
    }
}

// Traverses a curve of line segments defined by a list of input points and returns the indices of a subset of points
// which define a curve with strictly right handed or left handed orientation.
// The result always contains the start- and endpoints, therefore at least two input points are required.
// Implementation inspired by Graham scan for finding the convex hull of a point set, see https://en.wikipedia.org/wiki/Graham_scan
pub fn convex_envelope(points: &[SVector<f64, 2>], indices: &mut Vec<usize>, orientation: Orientation) {
    assert!(points.len() >= 2);
    assert!(orientation != Orientation::Collinear);

    indices.clear();
    indices.push(0);
    indices.push(1);

    let fits_as_next = |indices: &[usize], ic| {
        if indices.len() <= 1 {
            return true;
        }

        let ia = indices[indices.len() - 2];
        let ib = indices[indices.len() - 1];

        return Orientation::from_points(&points[ia], &points[ib], &points[ic]) == orientation;
    };

    for i in 2..points.len() {
        while !fits_as_next(&indices, i) {
            indices.pop();
        }

        indices.push(i);
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::vector;
    use super::*;

    #[test]
    fn test_point_orientation() {
        assert_eq!(Orientation::from_points(&vector![0.0, 0.0], &vector![1.0, 0.0], &vector![1.0, 1.0]), Orientation::LeftTurn);
        assert_eq!(Orientation::from_points(&vector![0.0, 0.0], &vector![0.0, 1.0], &vector![1.0, 1.0]), Orientation::RightTurn);
        assert_eq!(Orientation::from_points(&vector![0.0, 0.0], &vector![0.5, 0.5], &vector![1.0, 1.0]), Orientation::Collinear);
    }

    #[test]
    fn test_convex_envelope() {
        let mut output = Vec::new();

        // Two points should always be retained, no matter if left or right turn
        let input  = vec![vector![0.0, 0.0], vector![1.0, 0.0]];

        convex_envelope(&input, &mut output, Orientation::LeftTurn);
        assert_eq!(output, vec![0, 1]);

        convex_envelope(&input, &mut output, Orientation::RightTurn);
        assert_eq!(output, vec![0, 1]);

        // Three points that make a left turn should be retained for left target orientation
        // and reduced to the first and last point for right target orientation
        let input  = vec![vector![0.0, 0.0], vector![1.0, 0.0], vector![1.0, 1.0]];

        convex_envelope(&input, &mut output, Orientation::LeftTurn);
        assert_eq!(output, vec![0, 1, 2]);

        convex_envelope(&input, &mut output, Orientation::RightTurn);
        assert_eq!(output, vec![0, 2]);

        // Three points that make a right turn should be retained for right target orientation
        // and reduced to the first and last point for left target orientation
        let input  = vec![vector![0.0, 0.0], vector![0.0, 1.0], vector![1.0, 1.0]];

        convex_envelope(&input, &mut output, Orientation::RightTurn);
        assert_eq!(output, vec![0, 1, 2]);

        convex_envelope(&input, &mut output, Orientation::LeftTurn);
        assert_eq!(output, vec![0, 2]);

        // Three collinear points are reduced to the first and last for any target orientation
        let input  = vec![vector![0.0, 0.0], vector![0.5, 0.5], vector![1.0, 1.0]];

        convex_envelope(&input, &mut output, Orientation::LeftTurn);
        assert_eq!(output, vec![0, 2]);

        convex_envelope(&input, &mut output, Orientation::RightTurn);
        assert_eq!(output, vec![0, 2]);

        // Curves with left- and right turns are reduced to a subset according to the target orientation
        let input = vec![vector![0.0, 0.0], vector![0.5, -0.1], vector![1.0, 0.0], vector![1.5, 0.5], vector![2.0, 1.0], vector![2.5, 1.1], vector![3.0, 1.0]];

        convex_envelope(&input, &mut output, Orientation::LeftTurn);
        assert_eq!(output, vec![0, 1, 2, 6]);

        convex_envelope(&input, &mut output, Orientation::RightTurn);
        assert_eq!(output, vec![0, 4, 5, 6]);
    }
}