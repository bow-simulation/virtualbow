#pragma once
#include "TDMatrix.hpp"

#include <cassert>

class SplineFunction
{
public:
    bool init(const std::vector<double>& args, const std::vector<double>& values);
    double operator()(double arg) const;

    // Todo: Factor this out and make it work for all function objects
    void sample(std::vector<double>& args, std::vector<double>& values, size_t n_sample) const;

private:
    size_t interval_index(double arg) const;

    std::vector<double> t;
    std::vector<double> x;

    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> c;
};
