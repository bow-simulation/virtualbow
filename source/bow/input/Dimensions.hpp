#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

struct Dimensions
{
    double brace_height = 0.2;
    double draw_length = 0.7;
    double handle_length = 0.0;
    double handle_setback = 0.0;
    double handle_angle = 0.0;
    double stave_radius = 0.0;
};

static bool operator==(const Dimensions& lhs, const Dimensions& rhs)
{
    return lhs.brace_height == rhs.brace_height
        && lhs.draw_length == rhs.draw_length
        && lhs.handle_length == rhs.handle_length
        && lhs.handle_setback == rhs.handle_setback
        && lhs.handle_angle == rhs.handle_angle
        && lhs.stave_radius == rhs.stave_radius;
}

static bool operator!=(const Dimensions& lhs, const Dimensions& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Dimensions& value)
{
    obj["brace_height"] = value.brace_height;
    obj["draw_length"] = value.draw_length;
    obj["handle_length"] = value.handle_length;
    obj["handle_setback"] = value.handle_setback;
    obj["handle_angle"] = value.handle_angle;
    if (value.stave_radius>0) {
        obj["stave_radius"] = value.stave_radius;
    }
}

static void from_json(const json& obj, Dimensions& value)
{
    value.brace_height = obj.at("brace_height");
    value.draw_length = obj.at("draw_length");
    value.handle_length = obj.at("handle_length");
    value.handle_setback = obj.at("handle_setback");
    value.handle_angle = obj.at("handle_angle");
    value.stave_radius = obj.value("stave_radius", 0.0);
}
