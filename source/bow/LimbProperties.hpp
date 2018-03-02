#pragma once
#include "bow/input/InputData.hpp"
#include "LayerProperties.hpp"
#include "numerics/Eigen.hpp"
#include <json.hpp>

using nlohmann::json;

struct LimbProperties
{
    // Nodes
    VectorXd length;
    VectorXd angle;
    VectorXd x_pos;
    VectorXd y_pos;

    // Section geometry
    VectorXd width;
    VectorXd height;

    // Section properties
    VectorXd Cee;
    VectorXd Ckk;
    VectorXd Cek;
    VectorXd rhoA;

    // Layer properties
    std::vector<LayerProperties> layers;

    LimbProperties() = default;
    LimbProperties(const InputData& input);
    LimbProperties(const InputData& input, unsigned n);
};

static void to_json(json& obj, const LimbProperties& val)
{
    obj["length"] = val.length;
    obj["angle"] = val.angle;
    obj["x_pos"] = val.x_pos;
    obj["y_pos"] = val.y_pos;
    obj["width"] = val.width;
    obj["height"] = val.height;
    obj["Cee"] = val.Cee;
    obj["Ckk"] = val.Ckk;
    obj["Cek"] = val.Cek;
    obj["rhoA"] = val.rhoA;
    obj["layers"] = val.layers;
}
