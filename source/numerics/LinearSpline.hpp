#pragma once
#include "TDMatrix.hpp"
#include "DataSeries.hpp"

#include <cereal/cereal.hpp>    // Todo: Necessary hereß

class LinearSpline
{
public:
    LinearSpline(DataSeries data);

    double operator()(double arg) const;
    double arg_min() const;
    double arg_max() const;
    DataSeries sample(size_t n_points) const;

private:
    size_t interval_index(double arg) const;    // Todo: Common implementation for all interpolation classes
    DataSeries data;
};
