#pragma once
#include "bow/output/BowStates.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct StaticData
{
    BowStates states;
    double limb_mass = 0.0;
    double string_mass = 0.0;
    double string_length = 0.0;
    double final_draw_force = 0.0;
    double drawing_work = 0.0;
    double storage_ratio = 0.0;
    double max_string_force = 0.0;
    double max_strand_force = 0.0;
    double max_grip_force = 0.0;
    double max_draw_force = 0.0;
    std::vector<double> max_stress_pos;
    std::vector<double> max_stress_val;
};

static void to_json(json& obj, const StaticData& val)
{
    obj["states"] = val.states;
    obj["limb_mass"] = val.limb_mass;
    obj["string_mass"] = val.string_mass;
    obj["string_length"] = val.string_length;
    obj["final_draw_force"] = val.final_draw_force;
    obj["drawing_work"] = val.drawing_work;
    obj["storage_ratio"] = val.storage_ratio;

    obj["max_string_force"] = val.max_string_force;
    obj["max_strand_force"] = val.max_strand_force;
    obj["max_grip_force"] = val.max_grip_force;
    obj["max_draw_force"] = val.max_draw_force;
    obj["max_stress_pos"] = val.max_stress_pos;
    obj["max_stress_val"] = val.max_stress_val;
}
