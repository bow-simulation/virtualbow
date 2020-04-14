#pragma once
#include "solver/model/input/InputData.hpp"
#include "solver/numerics/Eigen.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct LayerProperties
{
    std::string name;
    double rho;
    double E;

    VectorXd length;

    MatrixXd He_back;
    MatrixXd Hk_back;

    MatrixXd He_belly;
    MatrixXd Hk_belly;

    LayerProperties() = default;

    // n: Limb nodex, k: Layer nodes
    LayerProperties(unsigned n, unsigned k)
        : length(VectorXd::Zero(k)),
          He_back(MatrixXd::Zero(k, n)),
          He_belly(MatrixXd::Zero(k, n)),
          Hk_back(MatrixXd::Zero(k, n)),
          Hk_belly(MatrixXd::Zero(k, n))
    {

    }
};

static void to_json(json& obj, const LayerProperties& val)
{
    obj["name"] = val.name;
    obj["rho"] = val.rho;
    obj["E"] = val.E;
    obj["length"] = val.length;
    obj["He_back"] = val.He_back;
    obj["Hk_back"] = val.Hk_back;
    obj["He_belly"] = val.He_belly;
    obj["Hk_belly"] = val.Hk_belly;
}

static void from_json(const json& obj, LayerProperties& val)
{
    val.name = obj.at("name");
    val.rho = obj.at("rho");
    val.E = obj.at("E");
    val.length = obj.at("length");
    val.He_back = obj.at("He_back");
    val.Hk_back = obj.at("Hk_back");
    val.He_belly = obj.at("He_belly");
    val.Hk_belly = obj.at("Hk_belly");
}

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

static void from_json(const json& obj, LimbProperties& val)
{
    val.length = obj.at("length");
    val.angle = obj.at("angle");
    val.x_pos = obj.at("x_pos");
    val.y_pos = obj.at("y_pos");
    val.width = obj.at("width");
    val.height = obj.at("height");
    val.rhoA = obj.at("rhoA");
    val.Cee = obj.at("Cee");
    val.Ckk = obj.at("Ckk");
    val.Cek = obj.at("Cek");
    val.layers = obj.at("layers").get<std::vector<LayerProperties>>();
}
