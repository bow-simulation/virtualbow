#pragma once
#include "model/output/SetupData.hpp"
#include "model/output/StaticData.hpp"
#include "model/output/DynamicData.hpp"
#include <json.hpp>
#include <fstream>
#include <vector>

using nlohmann::json;

struct OutputData
{
    SetupData setup;
    StaticData statics;
    DynamicData dynamics;

    void save(const std::string& path) const
    {
        std::vector<uint8_t> buffer = json::to_msgpack(*this);
        std::ofstream file(path);
        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }
};

static void to_json(json& obj, const OutputData& val)
{
    obj["setup"] = val.setup;
    obj["statics"] = val.statics;
    obj["dynamics"] = val.dynamics;
}

static void from_json(const json& obj, OutputData& val)
{
    val.setup = obj["setup"];
    val.statics = obj["statics"];
    val.dynamics = obj["dynamics"];
}
