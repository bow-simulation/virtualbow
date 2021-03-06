#pragma once
#include "Eigen.hpp"
#include <vector>
#include <cstddef>

// Cubic spline that preserves monotonicity of the input data.
// Implementation adapted from https://en.wikipedia.org/wiki/Monotone_cubic_interpolation
// Throws std::invalid_argument on invalid input
class CubicSpline
{
public:
    CubicSpline(const MatrixXd& input);
    double operator()(double x) const;
    double operator()(double x, double y_default) const;

    double arg_min() const;
    double arg_max() const;

private:
    mutable size_t index = 0;    // Interval index of last evaluation

    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<double> c1s;
    std::vector<double> c2s;
    std::vector<double> c3s;
};
