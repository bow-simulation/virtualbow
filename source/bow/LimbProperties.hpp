#pragma once
#include "LayerProperties.hpp"
#include "numerics/Eigen.hpp"
#include <json.hpp>

using nlohmann::json;

class InputData;

struct LimbProperties
{
    // Nodes
    VectorXd s;
    VectorXd x;
    VectorXd y;
    VectorXd phi;

    // Section geometry
    VectorXd w;
    VectorXd h;

    // Section properties
    VectorXd Cee;
    VectorXd Ckk;
    VectorXd Cek;
    VectorXd rhoA;

    // Layer properties
    std::vector<LayerProperties> layers;

    LimbProperties(const InputData& input);
    LimbProperties(const InputData& input, unsigned n);
    LimbProperties();
};

static void to_json(json& obj, const LimbProperties& val)
{
    obj["s"] = val.s;
    obj["x"] = val.x;
    obj["y"] = val.y;
}
