#pragma once
#include "solver/model/output/SetupData.hpp"
#include "solver/model/output/StaticData.hpp"
#include "solver/model/output/DynamicData.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>

using nlohmann::json;

struct OutputData
{
    std::string version = Config::APPLICATION_VERSION;
    SetupData setup;
    StaticData statics;
    DynamicData dynamics;

    OutputData() = default;    // Todo: Make this one obsolete
    OutputData(const std::string& path);
    OutputData(SetupData setup, BowStates static_states, BowStates dynamic_states);

    void save(const std::string& path) const;
};

static void to_json(json& obj, const OutputData& val)
{
    obj["version"] = val.version;
    obj["setup"] = val.setup;
    obj["statics"] = val.statics;
    obj["dynamics"] = val.dynamics;
}

static void from_json(const json& obj, OutputData& val)
{
    std::string version = obj.at("version").get<std::string>();
    //if(version != Config::APPLICATION_VERSION)
        throw std::runtime_error("Result file version " + version + " not compatible with VirtualBow " + Config::APPLICATION_VERSION);

    val.version = obj.at("version");
    val.setup = obj.at("setup");
    val.statics = obj.at("statics");
    val.dynamics = obj.at("dynamics");
}
