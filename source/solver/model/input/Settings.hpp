#pragma once
#include <nlohmann/json.hpp>

struct Settings
{
    unsigned n_limb_elements = 20;
    unsigned n_string_elements = 25;
    unsigned n_draw_steps = 150;
    double arrow_clamp_force = 0.5;
    double time_span_factor = 1.5;
    double time_step_factor = 0.2;
    double sampling_rate = 1e4;
};

static bool operator==(const Settings& lhs, const Settings& rhs)
{
    return lhs.n_limb_elements == rhs.n_limb_elements
        && lhs.n_string_elements == rhs.n_string_elements
        && lhs.n_draw_steps == rhs.n_draw_steps
        && lhs.arrow_clamp_force == rhs.arrow_clamp_force
        && lhs.time_span_factor == rhs.time_span_factor
        && lhs.time_step_factor == rhs.time_step_factor
        && lhs.sampling_rate == rhs.sampling_rate;
}

static bool operator!=(const Settings& lhs, const Settings& rhs)
{
    return !operator==(lhs, rhs);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Settings,
        n_limb_elements,
        n_string_elements,
        n_draw_steps,
        arrow_clamp_force,
        time_span_factor,
        time_step_factor,
        sampling_rate
)
