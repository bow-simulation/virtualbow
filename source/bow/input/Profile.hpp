#pragma once
#include "numerics/Series.hpp"
#include <json.hpp>

using nlohmann::json;

struct Profile
{
    Series segments = {{0.7}, {0.0}};
    double x0 = 0.0;
    double y0 = 0.0;
    double phi0 = 0.0;
};

static bool operator==(const Profile& lhs, const Profile& rhs)
{
    return lhs.segments == rhs.segments
        && lhs.x0 == rhs.x0
        && lhs.y0 == rhs.y0
        && lhs.phi0 == rhs.phi0;
}

static bool operator!=(const Profile& lhs, const Profile& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Profile& value)
{
    obj.at("segments") = value.segments;
    obj.at("x0") = value.x0;
    obj.at("y0") = value.y0;
    obj.at("phi0") = value.phi0;
}

static void from_json(const json& obj, Profile& value)
{
    value.segments = obj.at("segments");
    value.x0 = obj.at("x0");
    value.y0 = obj.at("y0");
    value.phi0 = obj.at("phi0");
}
