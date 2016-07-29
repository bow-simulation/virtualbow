#pragma once
#include "TDMatrix.hpp"
#include "DataSeries.hpp"

#include <cereal/cereal.hpp>

class SplineFunction
{
public:
    SplineFunction(DataSeries data);

    double operator()(double arg) const;
    DataSeries sample(size_t n_points) const;

private:
    size_t interval_index(double arg) const;

    std::vector<double> t;
    std::vector<double> x;

    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> c;
};
