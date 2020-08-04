#pragma once
#include "solver/numerics/Eigen.hpp"
#include <nlohmann/json.hpp>

struct Layer
{
    std::string name = "New layer";
    MatrixXd height{{0.0, 0.015}, {1.0, 0.01}};

    double rho = 600.0;
    double E = 15e9;
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
