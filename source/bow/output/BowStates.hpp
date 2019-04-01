#pragma once
#include "numerics/Eigen.hpp"
#include <vector>
#include <nlohmann/json.hpp>

using nlohmann::json;

struct BowStates
{
    std::vector<double> time;
    std::vector<double> draw_length;

    std::vector<double> draw_force;
    std::vector<double> string_force;
    std::vector<double> strand_force;
    std::vector<double> grip_force;

    std::vector<double> pos_arrow;
    std::vector<double> vel_arrow;
    std::vector<double> acc_arrow;

    std::vector<double> e_pot_limbs;
    std::vector<double> e_kin_limbs;
    std::vector<double> e_pot_string;
    std::vector<double> e_kin_string;
    std::vector<double> e_kin_arrow;

    std::vector<VectorXd> x_pos_limb;
    std::vector<VectorXd> y_pos_limb;
    std::vector<VectorXd> angle_limb;

    std::vector<VectorXd> x_pos_string;
    std::vector<VectorXd> y_pos_string;

    std::vector<VectorXd> epsilon;
    std::vector<VectorXd> kappa;
};

static void to_json(json& obj, const BowStates& val)
{
    obj["time"] = val.time;
    obj["draw_length"] = val.draw_length;
    obj["draw_force"] = val.draw_force;
    obj["string_force"] = val.string_force;
    obj["strand_force"] = val.strand_force;
    obj["grip_force"] = val.grip_force;
    obj["pos_arrow"] = val.pos_arrow;
    obj["vel_arrow"] = val.vel_arrow;
    obj["acc_arrow"] = val.acc_arrow;
    obj["e_pot_limbs"] = val.e_pot_limbs;
    obj["e_kin_limbs"] = val.e_kin_limbs;
    obj["e_pot_string"] = val.e_pot_string;
    obj["e_kin_string"] = val.e_kin_string;
    obj["e_kin_arrow"] = val.e_kin_arrow;
    obj["x_pos_limb"] = val.x_pos_limb;
    obj["y_pos_limb"] = val.y_pos_limb;
    obj["angle_limb"] = val.angle_limb;
    obj["x_pos_string"] = val.x_pos_string;
    obj["y_pos_string"] = val.y_pos_string;
    obj["epsilon"] = val.epsilon;
    obj["kappa"] = val.kappa;
}
