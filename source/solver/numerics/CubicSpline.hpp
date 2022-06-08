#pragma once
#include "EigenTypes.hpp"
#include <vector>

// Cubic spline that preserves monotonicity of the input data
// Throws std::invalid_argument on invalid input

enum class BoundaryType {
    FIRST_DERIVATIVE,
    SECOND_DERIVATIVE
};

class CubicSpline
{
public:
    CubicSpline(const std::vector<double>& args, const std::vector<double>& vals, bool monotonic = false,
                BoundaryType bd_type_left = BoundaryType::SECOND_DERIVATIVE, double bd_value_left = 0.0,
                BoundaryType bd_type_right = BoundaryType::SECOND_DERIVATIVE, double bd_value_right = 0.0);

    CubicSpline(const std::vector<Vector<2>>& input, bool monotonic = false,
                BoundaryType bd_type_left = BoundaryType::SECOND_DERIVATIVE, double bd_value_left = 0.0,
                BoundaryType bd_type_right = BoundaryType::SECOND_DERIVATIVE, double bd_value_right = 0.0);

    CubicSpline() = default;

    const std::vector<double>& args() const;
    const std::vector<double>& vals() const;

    double operator()(double arg) const;
    double operator()(double arg, double y_default) const;

    double deriv1(double arg) const;
    double deriv2(double arg) const;

    double arg_min() const;
    double arg_max() const;

private:
    mutable size_t index = 0;    // Interval index of last evaluation

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> m;
};
