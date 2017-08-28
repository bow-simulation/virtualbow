#pragma once
#include "model/output/BowStates.hpp"
#include <json.hpp>

using nlohmann::json;

struct StaticData
{
    BowStates states;
    double final_draw_force = 0.0;
    double drawing_work = 0.0;
    double storage_ratio = 0.0;
};

static void to_json(json& obj, const StaticData& val)
{
    obj["states"] = val.states;
    obj["final_draw_force"] = val.final_draw_force;
    obj["drawing_work"] = val.drawing_work;
    obj["storage_ratio"] = val.storage_ratio;
}

static void from_json(const json& obj, StaticData& val)
{
    val.states = obj["states"];
    val.final_draw_force = obj["final_draw_force"];
    val.drawing_work = obj["drawing_work"];
    val.storage_ratio = obj["storage_ratio"];
}
