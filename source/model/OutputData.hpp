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

struct StaticData
{
    const BowStates states;
    const double final_draw_force;
    const double drawing_work;
    const double storage_ratio;

    StaticData(BowStates states)
        : states(states),
          final_draw_force(states.draw_force.back()),
          drawing_work(states.e_pot_limbs.back() + states.e_pot_string.back() - states.e_pot_limbs.front() - states.e_pot_string.front()),
          storage_ratio(drawing_work/(0.5*(states.draw_length.back() - states.draw_length.front())*final_draw_force))
    {

    }
};

struct DynamicData
{
    const BowStates states;
    const double final_arrow_velocity;
    const double final_arrow_energy;
    const double efficiency;

    DynamicData(BowStates states, const StaticData& static_data)
        : states(states),
          final_arrow_velocity(std::abs(states.vel_arrow.back())),
          final_arrow_energy(states.e_kin_arrow.back()),
          efficiency(final_arrow_energy/static_data.drawing_work)
    {

    }
};

struct OutputData
{
    BowSetup setup;
    std::unique_ptr<StaticData> statics = nullptr;   // Todo: Use boost optional?
    std::unique_ptr<DynamicData> dynamics = nullptr;
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
