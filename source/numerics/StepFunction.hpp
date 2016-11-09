#pragma once
#include "Series.hpp"

#include <vector>
#include <algorithm>
#include <cassert>
#include <numeric>

using std::size_t;

class StepFunction
{
public:
    StepFunction(Series data);

    double operator()(double arg) const;
    double arg_min() const;
    double arg_max() const;
    Series sample() const;

private:
    size_t lower_index(double arg) const;

    std::vector<double> intervals;
    std::vector<double> values;
};
