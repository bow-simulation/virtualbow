#pragma once
#include "numerics/Eigen.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"
#include <vector>
#include <array>
#include <json.hpp>

using nlohmann::json;

// sigma_upper(s) = He_upper(s)*epsilon(s) + Hk_upper(s)*kappa(s)
// sigma_lower(s) = He_lower(s)*epsilon(s) + Hk_lower(s)*kappa(s)
struct LayerProperties
{
    double E;
    VectorXd s;
    VectorXd y_back;
    VectorXd y_belly;

    LayerProperties(size_t n);
    VectorXd sigma_back(const VectorXd& epsilon, const VectorXd& kappa) const;
    VectorXd sigma_belly(const VectorXd& epsilon, const VectorXd& kappa) const;
};

class InputData;

struct LimbProperties
{
    // Nodes
    VectorXd s;
    VectorXd x;
    VectorXd y;
    VectorXd w;
    VectorXd h;
    VectorXd phi;

    // Section properties
    VectorXd hc;          // Total cross section height (used for contact)
    VectorXd Cee;
    VectorXd Ckk;
    VectorXd Cek;
    VectorXd rhoA;

    // Layer properties
    std::vector<LayerProperties> layers;

    LimbProperties(const InputData& input);
    LimbProperties(const InputData& input, unsigned n_elements_limb);
    LimbProperties();
};

static void to_json(json& obj, const LimbProperties& val)
{
    obj["s"] = val.s;
    obj["x"] = val.x;
    obj["y"] = val.y;
}
