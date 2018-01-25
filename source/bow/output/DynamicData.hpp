#pragma once
#include "bow/output/BowStates.hpp"
#include <json.hpp>

using nlohmann::json;

struct DynamicData
{
    BowStates states;
    double final_arrow_velocity = 0.0;
    double final_arrow_energy = 0.0;
    double efficiency = 0.0;
};

static void to_json(json& obj, const DynamicData& val)
{
    obj["states"] = val.states;
    obj["final_arrow_velocity"] = val.final_arrow_velocity;
    obj["final_arrow_energy"] = val.final_arrow_energy;
    obj["efficiency"] = val.efficiency;
}
