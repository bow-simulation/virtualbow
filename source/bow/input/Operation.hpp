#pragma once
#include <json.hpp>

using nlohmann::json;

struct Operation
{
    double brace_height = 0.2;
    double draw_length = 0.7;
    double arrow_mass = 0.025;
};

static bool operator==(const Operation& lhs, const Operation& rhs)
{
    return lhs.brace_height == rhs.brace_height
        && lhs.draw_length == rhs.draw_length
        && lhs.arrow_mass == rhs.arrow_mass;
}

static bool operator!=(const Operation& lhs, const Operation& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Operation& value)
{
    obj["brace_height"] = value.brace_height;
    obj["draw_length"] = value.draw_length;
    obj["arrow_mass"] = value.arrow_mass;
}

static void from_json(const json& obj, Operation& value)
{
    value.brace_height = obj.at("brace_height");
    value.draw_length = obj.at("draw_length");
    value.arrow_mass = obj.at("arrow_mass");
}
