#pragma once
#include <json.hpp>

using nlohmann::json;

struct String2
{
    double strand_stiffness = 3500.0;
    double strand_density = 0.0005;
    double n_strands = 12.0;
};

static void to_json(json& obj, const String2& value)
{
    obj["strand_stiffness"] = value.strand_stiffness;
    obj["strand_density"] = value.strand_density;
    obj["n_strands"] = value.n_strands;
}

static void from_json(const json& obj, String2& value)
{
    value.strand_stiffness = obj["strand_stiffness"];
    value.strand_density = obj["strand_density"];
    value.n_strands = obj["n_strands"];
}
