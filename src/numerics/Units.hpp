#pragma once

#include <string>
#include <vector>
#include <cmath>

class Unit
{
public:
    Unit(const char* label, double factor)
        : label_(label), factor_(factor)
    {

    }

    const char* label() const
    {
        return label_;
    }

    double factor() const
    {
        return factor_;
    }

private:
    const char* label_;
    double factor_;
};

// base_value = input_value*unit_factor
namespace units
{
    const Unit one("", 1.0);
    const Unit percent("[%]", 1.0e-2);
    const Unit length("[mm]", 1.0e-3);
    const Unit angle("[°]", M_PI/180.0);
    const Unit mass("[g]", 1.0e-3);
    const Unit time("[s]", 1.0);
    const Unit velocity("[m/s]", 1.0);
    const Unit acceleration("[m/s²]", 1.0);
    const Unit force("[N]", 1.0);
    const Unit stress("[N/mm²]", 1.0e6);
    const Unit e_modulus("[N/mm²]", 1.0e6);
    const Unit strain("[%]", 1.0e-2);
    const Unit curvature("[1/mm]", 1.0e3);
    const Unit energy("[J]", 1.0);
    const Unit vol_density("[kg/m³]", 1.0);
    const Unit lin_density("[kg/m]", 1.0);
    const Unit ltd_stiffness("[N]", 1.0);
};
