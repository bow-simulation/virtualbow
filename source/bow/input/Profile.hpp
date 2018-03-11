#pragma once
#include "numerics/Series.hpp"
#include <json.hpp>

using nlohmann::json;

struct Profile
{
    Series segments = {{0.8}, {0.0}};
    double x_pos = 0.0;
    double y_pos = 0.0;
    double angle = 0.0;
};

static bool operator==(const Profile& lhs, const Profile& rhs)
{
    return lhs.segments == rhs.segments
        && lhs.x_pos == rhs.x_pos
        && lhs.y_pos == rhs.y_pos
        && lhs.angle == rhs.angle;
}

static bool operator!=(const Profile& lhs, const Profile& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Profile& value)
{
    obj["segments"] = value.segments;
    obj["x_pos"] = value.x_pos;
    obj["y_pos"] = value.y_pos;
    obj["angle"] = value.angle;
}

static void from_json(const json& obj, Profile& value)
{
    value.segments = obj.at("segments");
    value.x_pos = obj.at("x_pos");
    value.y_pos = obj.at("y_pos");
    value.angle = obj.at("angle");
}
