#pragma once
#include <nlohmann/json.hpp>

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Damping, damping_ratio_limbs, damping_ratio_string)
