#pragma once
#include <vector>
#include <valarray>
#include <json.hpp>

using nlohmann::json;

struct BowStates
{
    std::vector<double> time;
    std::vector<double> draw_force;
    std::vector<double> draw_length;

    std::vector<double> pos_arrow;
    std::vector<double> vel_arrow;
    std::vector<double> acc_arrow;

    std::vector<double> e_pot_limbs;
    std::vector<double> e_kin_limbs;
    std::vector<double> e_pot_string;
    std::vector<double> e_kin_string;
    std::vector<double> e_kin_arrow;

    std::vector<double> y_arrow;
    std::vector<std::valarray<double>> x_limb;
    std::vector<std::valarray<double>> y_limb;
    std::vector<std::valarray<double>> x_string;
    std::vector<std::valarray<double>> y_string;

    std::vector<std::valarray<double>> sigma_upper;
    std::vector<std::valarray<double>> sigma_lower;
};

static void to_json(json& obj, const BowStates& val)
{
    obj["time"] = val.time;
    obj["draw_force"] = val.draw_force;
    obj["draw_length"] = val.draw_length;
    obj["pos_arrow"] = val.pos_arrow;
    obj["vel_arrow"] = val.vel_arrow;
    obj["acc_arrow"] = val.acc_arrow;
    obj["e_pot_limbs"] = val.e_pot_limbs;
    obj["e_kin_limbs"] = val.e_kin_limbs;
    obj["e_pot_string"] = val.e_pot_string;
    obj["e_kin_string"] = val.e_kin_string;
    obj["e_kin_arrow"] = val.e_kin_arrow;
    obj["y_arrow"] = val.y_arrow;
    obj["x_limb"] = val.x_limb;
    obj["y_limb"] = val.y_limb;
    obj["x_string"] = val.x_string;
    obj["y_string"] = val.y_string;
    obj["sigma_upper"] = val.sigma_upper;
    obj["sigma_lower"] = val.sigma_lower;
}
