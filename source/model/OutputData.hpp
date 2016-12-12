#pragma once
#include "DiscreteLimb.hpp"
#include <memory>
#include <algorithm>

struct BowSetup
{
    DiscreteLimb limb;
    double string_length;
};

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

    std::vector<std::vector<double>> x_limb;
    std::vector<std::vector<double>> y_limb;
    std::vector<std::vector<double>> x_string;
    std::vector<std::vector<double>> y_string;
    std::vector<double> y_arrow;

    std::vector<std::vector<double>> sigma_upper;
    std::vector<std::vector<double>> sigma_lower;

    BowStates();    // Todo: Why?
};

struct StaticData
{
    const BowStates states;
    const double final_draw_force;
    const double drawing_work;
    const double storage_ratio;

    StaticData(BowStates states);
};

struct DynamicData
{
    const BowStates states;
    const double final_arrow_velocity;
    const double final_arrow_energy;
    const double efficiency;

    DynamicData(BowStates states, const StaticData& static_data);
};

struct OutputData
{
    BowSetup setup;
    std::unique_ptr<StaticData> statics = nullptr;   // Todo: Use boost optional?
    std::unique_ptr<DynamicData> dynamics = nullptr;
};
