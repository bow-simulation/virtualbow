#pragma once
#include "Eigen.hpp"
#include <vector>
#include <cstddef>

// Cubic spline that preserves monotonicity of the input data.
// Implementation adapted from [1]
//
// [1] George Wolfberg, Itzik Alfx: An energy-minimization framework for monotonic cubic spline interpolation
//     Journal of Computational and Applied Mathematics, Vol. 143, No. 2, pages 145-188, 2002.
//     https://www.sciencedirect.com/science/article/pii/S0377042701005064

class CubicSpline
{
public:
    CubicSpline(const MatrixXd& input);

    double operator()(double arg) const;
    double operator()(double arg, double default_value) const;

    double arg_min() const;
    double arg_max() const;

private:
    mutable size_t index = 0;    // Interval index of last evaluation

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> m;
};
