#pragma once
#include "bow/LimbProperties.hpp"
#include "bow/output/StaticData.hpp"
#include "bow/output/DynamicData.hpp"
#include <json.hpp>
#include <fstream>
#include <vector>

using nlohmann::json;

struct OutputData
{
    LimbProperties limb_properties;
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
    obj["limb_properties"] = val.limb_properties;
    obj["statics"] = val.statics;
    obj["dynamics"] = val.dynamics;
}
