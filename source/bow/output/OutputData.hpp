#pragma once
#include "bow/LimbProperties.hpp"
#include "bow/output/StaticData.hpp"
#include "bow/output/DynamicData.hpp"
#include <nlohmann/json.hpp>
#include <vector>

using nlohmann::json;

struct OutputData
{
    LimbProperties limb_properties;
    StaticData statics;
    DynamicData dynamics;

    // Todo: Move this somewhere else, maybe in Application class. Interface of OutputData can be just json.
    void save(const std::string& path) const;
};

static void to_json(json& obj, const OutputData& val)
{
    obj["limb_properties"] = val.limb_properties;
    obj["statics"] = val.statics;
    obj["dynamics"] = val.dynamics;
}
