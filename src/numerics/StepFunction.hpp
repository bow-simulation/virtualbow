#pragma once

#include <vector>
#include <algorithm>
#include <cassert>

using std::size_t;

class StepFunction
{
private:
    std::vector<double> intervals;
    std::vector<double> values;

public:
    StepFunction(std::vector<double> lengths, std::vector<double> values);
    double operator()(double arg) const;

private:
    size_t lower_index(double arg) const;
};
