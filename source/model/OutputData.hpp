#pragma once
#include "DiscreteLimb.hpp"
#include <memory>


struct SpecialValue
{
    double value;
    size_t i;       // Index of the bow state
    size_t j;       // Index along the limb axis
};

struct BowSetup
{
    DiscreteLimb limb;
    double string_length;
};

struct BowStates
{
    std::vector<double> time;
    std::vector<double> draw_length;
    std::vector<double> draw_force;

    std::vector<double> pos_arrow;
    std::vector<double> vel_arrow;
    std::vector<double> acc_arrow;

    std::vector<double> pos_string;
    std::vector<double> vel_string;
    std::vector<double> acc_string;

    std::vector<double> e_pot_limbs;
    std::vector<double> e_kin_limbs;
    std::vector<double> e_pot_string;
    std::vector<double> e_kin_string;
    std::vector<double> e_kin_arrow;

    std::vector<std::vector<double>> x_limb;
    std::vector<std::vector<double>> y_limb;
    std::vector<std::vector<double>> x_string;
    std::vector<std::vector<double>> y_string;
    std::vector<std::vector<double>> sigma_upper;
    std::vector<std::vector<double>> sigma_lower;

    BowStates() // Todo: Why?
    {

    }
};

struct OutputData
{
    BowSetup setup;
    std::unique_ptr<BowStates> statics = nullptr;   // Todo: Use boost optional?
    std::unique_ptr<BowStates> dynamics = nullptr;
};


/*
class BowState
{
    double time;
    double draw_force;

    double pos_arrow;
    double vel_arrow;
    double acc_arrow;

    double pos_string_center;
    double vel_string_center;
    double acc_string_center;

    std::vector<double> x_limb;
    std::vector<double> y_limb;
    std::vector<double> x_string;
    std::vector<double> y_string;
    std::vector<double> sigma_upper;
    std::vector<double> sigma_lower;
};
*/
