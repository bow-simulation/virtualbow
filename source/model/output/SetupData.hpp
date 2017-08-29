#pragma once
#include "model/LimbProperties.hpp"
#include <json.hpp>

using nlohmann::json;

struct SetupData
{
    LimbProperties limb;
    double string_length = 0.0;
};

static void to_json(json& obj, const SetupData& val)
{
    obj["limb"] = val.limb;
    obj["string_length"] = val.string_length;
}

static void from_json(const json& obj, SetupData& val)
{
    val.limb = obj["limb"];
    val.string_length = obj["string_length"];
}
