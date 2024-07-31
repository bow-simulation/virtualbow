#pragma once
#include "solver/model/output/BowStates.hpp"
#include <nlohmann/json.hpp>
#include <utility>

struct StaticData
{
    BowStates states;

    double final_draw_force = 0.0;
    double drawing_work = 0.0;
    double energy_storage_factor = 0.0;

    unsigned max_string_force_index = 0;
    unsigned max_grip_force_index = 0;
    unsigned max_draw_force_index = 0;

    std::vector<double> min_stress_value;
    std::vector<std::pair<unsigned, unsigned>> min_stress_index;

    std::vector<double> max_stress_value;
    std::vector<std::pair<unsigned, unsigned>> max_stress_index;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        StaticData,
        states,
        final_draw_force,
        drawing_work,
        energy_storage_factor,
        max_string_force_index,
        max_grip_force_index,
        max_draw_force_index,
        min_stress_value,
        min_stress_index,
        max_stress_value,
        max_stress_index
)
