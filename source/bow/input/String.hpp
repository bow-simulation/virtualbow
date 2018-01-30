#pragma once
#include <json.hpp>

using nlohmann::json;

struct String
{
    double strand_stiffness = 3500.0;
    double strand_density = 0.0005;
    int n_strands = 12;
};

static bool operator==(const String& lhs, const String& rhs)
{
    return lhs.strand_stiffness == rhs.strand_stiffness
        && lhs.strand_density == rhs.strand_density
        && lhs.n_strands == rhs.n_strands;
}

static bool operator!=(const String& lhs, const String& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const String& value)
{
    obj["strand_stiffness"] = value.strand_stiffness;
    obj["strand_density"] = value.strand_density;
    obj["n_strands"] = value.n_strands;
}

static void from_json(const json& obj, String& value)
{
    value.strand_stiffness = obj.at("strand_stiffness");
    value.strand_density = obj.at("strand_density");
    value.n_strands = obj.at("n_strands");
}
