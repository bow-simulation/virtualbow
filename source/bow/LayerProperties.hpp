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

    // n: Limb nodex, k: Layer nodes
    LayerProperties(unsigned n, unsigned k)
        : s(VectorXd::Zero(k)),
          y_back(VectorXd::Zero(k)),
          y_belly(VectorXd::Zero(k)),
          He_back(MatrixXd::Zero(k, n)),
          He_belly(MatrixXd::Zero(k, n)),
          Hk_back(MatrixXd::Zero(k, n)),
          Hk_belly(MatrixXd::Zero(k, n))
    {

    }

    VectorXd sigma_back(const VectorXd& epsilon, const VectorXd& kappa) const
    {
        return He_back*epsilon + Hk_back*kappa;
    }

    VectorXd sigma_belly(const VectorXd& epsilon, const VectorXd& kappa) const
    {
        return He_belly*epsilon + Hk_belly*kappa;
    }
};
