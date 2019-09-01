#pragma once
#include "bow/input/InputData.hpp"
#include "LayerProperties.hpp"
#include "numerics/Eigen.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct LimbProperties
{
    // Nodes
    VectorXd length;
    VectorXd x_pos;
    VectorXd y_pos;
    VectorXd angle;

    // Section geometry
    VectorXd width;
    VectorXd height;

    // Section properties
    VectorXd rhoA;
    VectorXd Cee;
    VectorXd Ckk;
    VectorXd Cek;

    // Segment properties
    std::vector<Matrix<6, 6>> K;
    std::vector<double> m;

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
    obj["rhoA"] = val.rhoA;
    obj["Cee"] = val.Cee;
    obj["Ckk"] = val.Ckk;
    obj["Cek"] = val.Cek;
    obj["layers"] = val.layers;
}
