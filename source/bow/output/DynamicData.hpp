#pragma once
#include "bow/output/BowStates.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct DynamicData
{
    BowStates states;
    double final_arrow_velocity = 0.0;
    double final_arrow_energy = 0.0;
    double efficiency = 0.0;
    double max_string_force = 0.0;
    double max_strand_force = 0.0;
    double max_grip_force = 0.0;
    double max_draw_force = 0.0;
    std::vector<double> max_stress_pos;
    std::vector<double> max_stress_val;
};

static void to_json(json& obj, const DynamicData& val)
{
    obj["states"] = val.states;
    obj["final_arrow_velocity"] = val.final_arrow_velocity;
    obj["final_arrow_energy"] = val.final_arrow_energy;
    obj["efficiency"] = val.efficiency;
    obj["max_string_force"] = val.max_string_force;
    obj["max_strand_force"] = val.max_strand_force;
    obj["max_grip_force"] = val.max_grip_force;
    obj["max_draw_force"] = val.max_draw_force;
    obj["max_stress_pos"] = val.max_stress_pos;
    obj["max_stress_val"] = val.max_stress_val;
}
