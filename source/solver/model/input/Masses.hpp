#pragma once
#include <nlohmann/json.hpp>

struct Masses
{
    double arrow = 0.025;
    double string_center = 0.0;
    double string_tip = 0.0;
    double limb_tip = 0.0;
};

static bool operator==(const Masses& lhs, const Masses& rhs)
{
    return lhs.arrow == rhs.arrow
        && lhs.string_center == rhs.string_center
        && lhs.string_tip == rhs.string_tip
        && lhs.limb_tip == rhs.limb_tip;
}

static bool operator!=(const Masses& lhs, const Masses& rhs)
{
    return !operator==(lhs, rhs);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Masses, arrow, string_center, string_tip, limb_tip)
