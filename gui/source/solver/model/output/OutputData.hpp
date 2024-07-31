#pragma once
#include "solver/model/output/SetupData.hpp"
#include "solver/model/output/StaticData.hpp"
#include "solver/model/output/DynamicData.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OutputData, version, setup, statics, dynamics)
