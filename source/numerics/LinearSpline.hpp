#pragma once
#include "TDMatrix.hpp"
#include "Series.hpp"

class LinearSpline
{
public:
    LinearSpline(Series data);

    double operator()(double arg) const;
    double arg_min() const;
    double arg_max() const;
    Series sample(size_t n_points) const;

private:
    size_t interval_index(double arg) const;    // Todo: Common implementation for all interpolation classes
    Series data;
};
