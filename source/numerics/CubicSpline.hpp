#pragma once
#include "Series.hpp"

#include <vector>
#include <stdexcept>
#include <algorithm>

#include <iostream>

// Cubic spline that preserves monotonicity of the input data.
// Implementation adapted from https://en.wikipedia.org/wiki/Monotone_cubic_interpolation

class CubicSpline
{
public:
    CubicSpline(const Series& data);
    double operator()(double x, double y_default = 0.0) const;
    Series sample(size_t n);

private:
    bool is_strictly_increasing(const std::vector<double>& args);
    std::vector<double> interpolate(const std::vector<double>& args);

    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<double> c1s;
    std::vector<double> c2s;
    std::vector<double> c3s;
};
