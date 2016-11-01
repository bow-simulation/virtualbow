#pragma once
#include "TDMatrix.hpp"
#include "Series.hpp"

class CubicSpline
{
public:
    CubicSpline(Series data);

    double operator()(double arg) const;
    double arg_min() const;
    double arg_max() const;
    Series sample(size_t n_points) const;

private:
    size_t interval_index(double arg) const;

    std::vector<double> t;
    std::vector<double> x;

    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> c;
};
