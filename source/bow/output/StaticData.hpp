#pragma once
#include "bow/output/BowStates.hpp"
#include <json.hpp>

using nlohmann::json;

struct StaticData
{
    BowStates states;
    double string_length = 0.0;
    double final_draw_force = 0.0;
    double drawing_work = 0.0;
    double storage_ratio = 0.0;
};

static void to_json(json& obj, const StaticData& val)
{
    obj["states"] = val.states;
    obj["string_length"] = val.string_length;
    obj["final_draw_force"] = val.final_draw_force;
    obj["drawing_work"] = val.drawing_work;
    obj["storage_ratio"] = val.storage_ratio;
}
