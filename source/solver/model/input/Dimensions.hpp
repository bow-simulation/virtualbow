#pragma once
#include <nlohmann/json.hpp>

struct Dimensions
{
    double brace_height = 0.2;
    double draw_length = 0.7;
    double handle_length = 0.0;
    double handle_setback = 0.0;
    double handle_angle = 0.0;
};

static bool operator==(const Dimensions& lhs, const Dimensions& rhs)
{
    return lhs.brace_height == rhs.brace_height
        && lhs.draw_length == rhs.draw_length
        && lhs.handle_length == rhs.handle_length
        && lhs.handle_setback == rhs.handle_setback
        && lhs.handle_angle == rhs.handle_angle;
}

static bool operator!=(const Dimensions& lhs, const Dimensions& rhs)
{
    return !operator==(lhs, rhs);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Dimensions, brace_height, draw_length, handle_length, handle_setback, handle_angle)
