#pragma once
#include "numerics/Eigen.hpp"

// sigma_upper = He_upper*epsilon + Hk_upper*kappa
// sigma_lower = He_lower*epsilon + Hk_lower*kappa

struct LayerProperties
{
    VectorXd s;
    VectorXd y_back;
    VectorXd y_belly;
};
