#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

struct Settings
{
    unsigned n_limb_elements = 40;
    unsigned n_string_elements = 45;
    unsigned n_draw_steps = 150;
    double time_span_factor = 1.5;
    double time_step_factor = 0.5;
    double sampling_rate = 1e4;
};

static bool operator==(const Settings& lhs, const Settings& rhs)
{
    return lhs.n_limb_elements == rhs.n_limb_elements
        && lhs.n_string_elements == rhs.n_string_elements
        && lhs.n_draw_steps == rhs.n_draw_steps
        && lhs.time_span_factor == rhs.time_span_factor
        && lhs.time_step_factor == rhs.time_step_factor
        && lhs.sampling_rate == rhs.sampling_rate;
}

static bool operator!=(const Settings& lhs, const Settings& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Settings& value)
{
    obj["n_limb_elements"] = value.n_limb_elements;
    obj["n_string_elements"] = value.n_string_elements;
    obj["n_draw_steps"] = value.n_draw_steps;
    obj["time_span_factor"] = value.time_span_factor;
    obj["time_step_factor"] = value.time_step_factor;
    obj["sampling_rate"] = value.sampling_rate;
}

static void from_json(const json& obj, Settings& value)
{
    value.n_limb_elements = obj.at("n_limb_elements");
    value.n_string_elements = obj.at("n_string_elements");
    value.n_draw_steps = obj.at("n_draw_steps");
    value.time_span_factor = obj.at("time_span_factor");
    value.time_step_factor = obj.at("time_step_factor");
    value.sampling_rate = obj.at("sampling_rate");
}
