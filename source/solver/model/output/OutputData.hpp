#pragma once
#include "solver/model/output/SetupData.hpp"
#include "solver/model/output/StaticData.hpp"
#include "solver/model/output/DynamicData.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>

using nlohmann::json;

struct OutputData
{
    SetupData setup;
    StaticData statics;
    DynamicData dynamics;

    // Todo: Make this one obsolete
    OutputData() = default;
    OutputData(const std::string& path);
    OutputData(SetupData setup, BowStates static_states, BowStates dynamic_states);
    void save(const std::string& path) const;
};

static void to_json(json& obj, const OutputData& val)
{
    obj["setup"] = val.setup;
    obj["statics"] = val.statics;
    obj["dynamics"] = val.dynamics;
}

static void from_json(const json& obj, OutputData& val)
{
    val.setup = obj.at("setup");
    val.statics = obj.at("statics");
    val.dynamics = obj.at("dynamics");
}
