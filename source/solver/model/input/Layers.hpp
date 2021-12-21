#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include "solver/numerics/EigenSerialize.hpp"

struct Layer
{
    std::string name = "New layer";
    MatrixXd height{{0.0, 0.015}, {1.0, 0.01}};

    double rho = 675.0;
    double E = 12e9;
};

static bool operator==(const Layer& lhs, const Layer& rhs)
{
    return lhs.name == rhs.name
        && lhs.height == rhs.height
        && lhs.rho == rhs.rho
        && lhs.E == rhs.E;
}

static bool operator!=(const Layer& lhs, const Layer& rhs)
{
    return !operator==(lhs, rhs);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Layer, name, height, rho, E)
