#pragma once
#include <json.hpp>

using nlohmann::json;

struct Settings
{
    int n_elements_limb = 40;
    int n_elements_string = 45;
    int n_draw_steps = 150;
    double time_span_factor = 1.5;
    double time_step_factor = 0.5;
    double sampling_rate = 1e4;
};

static bool operator==(const Settings& lhs, const Settings& rhs)
{
    return lhs.n_elements_limb == rhs.n_elements_limb
        && lhs.n_elements_string == rhs.n_elements_string
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
    obj["n_elements_limb"] = value.n_elements_limb;
    obj["n_elements_string"] = value.n_elements_string;
    obj["n_draw_steps"] = value.n_draw_steps;
    obj["time_span_factor"] = value.time_span_factor;
    obj["time_step_factor"] = value.time_step_factor;
    obj["sampling_rate"] = value.sampling_rate;
}

static void from_json(const json& obj, Settings& value)
{
    value.n_elements_limb = obj["n_elements_limb"];
    value.n_elements_string = obj["n_elements_string"];
    value.n_draw_steps = obj["n_draw_steps"];
    value.time_span_factor = obj["time_span_factor"];
    value.time_step_factor = obj["time_step_factor"];
    value.sampling_rate = obj["sampling_rate"];
}
