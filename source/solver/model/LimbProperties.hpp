#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include "solver/numerics/EigenSerialize.hpp"
#include <nlohmann/json.hpp>
#include <vector>

class InputData;

struct LayerProperties
{
    std::string name;
    std::string color;
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LayerProperties, name, color, rho, E, length, He_back, Hk_back, He_belly, Hk_belly)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LimbProperties, length, angle, x_pos, y_pos, width, height, rhoA, Cee, Ckk, Cek, layers)
