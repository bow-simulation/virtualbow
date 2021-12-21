#pragma once
#include "EigenTypes.hpp"
#include <vector>

enum class Orientation {
    LeftHanded,
    RightHanded,
    Collinear
};

// Calculates the orientation of the triangle {a, b, c}.
Orientation get_orientation(const Vector<2>& a, const Vector<2>& b, const Vector<2>& c);

// Traverses a curve given by a list of input points and returns a subset of points with
// strictly right handed or left handed orientation that contains the start- and endpoints.
// Implementation inspired by Graham scan for finding the convex hull of a point set
// (https://en.wikipedia.org/wiki/Graham_scan)
std::vector<Vector<2>> constant_orientation_subset(const std::vector<Vector<2>>& input, Orientation orientation);

// Partitions a curve of linear segments given by a list of points into n_out points such that
// the first and last point coincide with the start and end of the curve and all points
// are evenly spaced out by euclidean distance.
std::vector<Vector<2>> equipartition(const std::vector<Vector<2>>& input, size_t n_out);
