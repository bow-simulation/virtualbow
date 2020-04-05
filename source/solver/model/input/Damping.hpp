#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

struct Damping
{
    double damping_ratio_limbs = 0.0;
    double damping_ratio_string = 0.0;
};

static bool operator==(const Damping& lhs, const Damping& rhs)
{
    return lhs.damping_ratio_limbs == rhs.damping_ratio_limbs
        && lhs.damping_ratio_string == rhs.damping_ratio_string;
}

static bool operator!=(const Damping& lhs, const Damping& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Damping& value)
{
    obj["damping_ratio_limbs"] = value.damping_ratio_limbs;
    obj["damping_ratio_string"] = value.damping_ratio_string;
}

static void from_json(const json& obj, Damping& value)
{
    value.damping_ratio_limbs = obj.at("damping_ratio_limbs");
    value.damping_ratio_string = obj.at("damping_ratio_string");
}

