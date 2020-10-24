#pragma once
#include <vector>

// Lookup table with linear interpolation
class LinearSpline {
public:
    LinearSpline() = default;
    LinearSpline(const std::vector<double>& args, const std::vector<double>& vals);

    double arg_min() const;
    double arg_max() const;

    double operator()(double arg) const;
    double operator()(double arg, double default_value) const;

private:
    mutable size_t index = 0;    // Interval index of last evaluation
    std::vector<double> x;
    std::vector<double> y;
};
