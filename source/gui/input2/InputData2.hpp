#pragma once

#include "numerics/Series.hpp"
#include <string>

struct Layer2
{
    std::string name = "";
    Series height = {{0.0, 1.0}, {0.01, 0.01}};
    double rho = 5000.0;
    double E = 1.0e12;
};
