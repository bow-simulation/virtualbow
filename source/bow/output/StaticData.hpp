#pragma once
#include "bow/output/BowStates.hpp"
#include <nlohmann/json.hpp>
#include <utility>

using nlohmann::json;

struct StaticData
{
    BowStates states;

    double final_draw_force = 0.0;
    double drawing_work = 0.0;
    double storage_ratio = 0.0;

    unsigned max_string_force_index = 0;
    unsigned max_grip_force_index = 0;
    unsigned max_draw_force_index = 0;

    std::vector<double> max_stress_value;
    std::vector<std::pair<unsigned, unsigned>> max_stress_index;
};

static void to_json(json& obj, const StaticData& val)
{
    obj["states"] = val.states;
    obj["final_draw_force"] = val.final_draw_force;
    obj["drawing_work"] = val.drawing_work;
    obj["storage_ratio"] = val.storage_ratio;
    obj["max_string_force_index"] = val.max_string_force_index;
    obj["max_grip_force_index"] = val.max_grip_force_index;
    obj["max_draw_force_index"] = val.max_draw_force_index;
    obj["max_stress_value"] = val.max_stress_value;
    obj["max_stress_index"] = val.max_stress_index;
}
