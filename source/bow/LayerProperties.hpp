#pragma once
#include "numerics/Eigen.hpp"

// sigma_upper = He_upper*epsilon + Hk_upper*kappa
// sigma_lower = He_lower*epsilon + Hk_lower*kappa

struct LayerProperties
{
    VectorXd s;

    VectorXd y_back;
    VectorXd y_belly;

    MatrixXd He_back;
    MatrixXd He_belly;

    MatrixXd Hk_back;
    MatrixXd Hk_belly;

    VectorXd sigma_back(const VectorXd& epsilon, const VectorXd& kappa) const
    {
        return He_back*epsilon + Hk_back*kappa;
    }

    VectorXd sigma_belly(const VectorXd& epsilon, const VectorXd& kappa) const
    {
        return He_belly*epsilon + Hk_belly*kappa;
    }
};
