#pragma once
#include "numerics/Eigen.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct LayerProperties
{
    VectorXd length;

    // Todo: Remove when limb view doesn't use this anymore
    VectorXd y_back;
    VectorXd y_belly;

    MatrixXd He_back;
    MatrixXd Hk_back;

    MatrixXd He_belly;
    MatrixXd Hk_belly;

    // n: Limb nodex, k: Layer nodes
    LayerProperties(unsigned n, unsigned k)
        : length(VectorXd::Zero(k)),
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

static void to_json(json& obj, const LayerProperties& val)
{
    obj["length"] = val.length;
    obj["He_back"] = val.He_back;
    obj["Hk_back"] = val.Hk_back;
    obj["He_belly"] = val.He_belly;
    obj["Hk_belly"] = val.Hk_belly;
}
