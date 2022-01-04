#pragma once
#include "EigenTypes.hpp"
#include "solver/numerics/tkspline/spline.h"
#include <vector>

// Cubic spline that preserves monotonicity of the input data
// Throws std::invalid_argument on invalid input
class CubicSpline
{
public:
    CubicSpline(const std::vector<Vector<2>>& input);
    double operator()(double x) const;
    double operator()(double x, double y_default) const;

    double arg_min() const;
    double arg_max() const;

private:
    tk::spline spline;
};
