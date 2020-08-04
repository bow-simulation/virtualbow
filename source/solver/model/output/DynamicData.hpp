#pragma once
#include "solver/model/output/BowStates.hpp"
#include <nlohmann/json.hpp>

struct DynamicData
{
    BowStates states;

    double final_pos_arrow = 0;
    double final_vel_arrow = 0;

    double final_e_pot_limbs = 0;
    double final_e_kin_limbs = 0;
    double final_e_pot_string = 0;
    double final_e_kin_string = 0;
    double final_e_kin_arrow = 0;

    double efficiency = 0.0;

    unsigned max_string_force_index = 0;
    unsigned max_grip_force_index = 0;
    unsigned arrow_departure_index = 0;

    std::vector<double> max_stress_value;
    std::vector<std::pair<unsigned, unsigned>> max_stress_index;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DynamicData, states, final_pos_arrow, final_vel_arrow, final_e_pot_limbs, final_e_kin_limbs, final_e_pot_string,
        final_e_kin_string, final_e_kin_arrow, efficiency, max_string_force_index, max_grip_force_index, max_stress_value, max_stress_index)
