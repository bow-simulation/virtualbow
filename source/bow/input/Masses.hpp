#pragma once
#include <json.hpp>

using nlohmann::json;

struct Masses
{
    double string_center = 0.005;
    double string_tip = 0.005;
    double limb_tip = 0.005;
};

static bool operator==(const Masses& lhs, const Masses& rhs)
{
    return lhs.string_center == rhs.string_center
        && lhs.string_tip == rhs.string_tip
        && lhs.limb_tip == rhs.limb_tip;
}

static void to_json(json& obj, const Masses& value)
{
    obj["string_center"] = value.string_center;
    obj["string_tip"] = value.string_tip;
    obj["limb_tip"] = value.limb_tip;
}

static void from_json(const json& obj, Masses& value)
{
    value.string_center = obj["string_center"];
    value.string_tip = obj["string_tip"];
    value.limb_tip = obj["limb_tip"];
}

