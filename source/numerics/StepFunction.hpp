#pragma once
#include "DataSeries.hpp"

#include <cereal/cereal.hpp>

#include <vector>
#include <algorithm>
#include <cassert>

using std::size_t;

class StepFunction
{
public:
    StepFunction(DataSeries data);

    double operator()(double arg) const;
    double arg_min() const;
    double arg_max() const;
    DataSeries sample() const;

private:
    size_t lower_index(double arg) const;

    std::vector<double> intervals;
    std::vector<double> values;
};
