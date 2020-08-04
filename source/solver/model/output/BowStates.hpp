#pragma once
#include "solver/numerics/Eigen.hpp"
#include <vector>
#include <nlohmann/json.hpp>

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BowStates, time, draw_length, draw_force, string_force, strand_force, grip_force, pos_arrow, vel_arrow, acc_arrow,
        e_pot_limbs, e_kin_limbs, e_pot_string, e_kin_string, e_kin_arrow, x_pos_limb, y_pos_limb, angle_limb, x_pos_string, y_pos_string, epsilon, kappa)
