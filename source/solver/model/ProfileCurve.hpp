#pragma once
#include "solver/numerics/Eigen.hpp"

class ProfileCurve
{
public:
    // s: Arc lengths of the profile curve in ascending order
    // k: Curvature of the profile curve at the specified arc lengths
    // x0, y0, phi0: Starting point and angle
    // Throws std::invalid_argument on invalid input
    ProfileCurve(std::vector<double> s, std::vector<double> k, double x0, double y0, double phi0);

    // Arc length at start and end of the curve
    double s_min() const;
    double s_max() const;

    // Evaluate curve at a specific arc length. Returns {x, y, phi}.
    Vector<3> operator()(double arg) const;

private:
    using segment_fn= std::function<Vector<3>(double)>;

    mutable size_t index = 0;    // Interval index of last evaluation
    std::vector<double> s;       // Arc lengths of the intervals
    std::vector<segment_fn> f;   // Curve functions for each interval

    // Returns a function s -> {x, y, phi} that describes a curve with linearly varying curvature k over arc length s.
    // s0, s1, k0, k1 : Arc length and curvature at start and end of the curve
    // x0, y0, phi0 : Starting point and angle
    segment_fn euler_spiral(double s0, double s1, double k0, double k1, double x0, double y0, double phi0);
};
