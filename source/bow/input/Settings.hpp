#pragma once
#include "bow/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Settings: public DocumentNode
{
    DocumentItem<int>     n_elements_limb{*this, 40};
    DocumentItem<int>   n_elements_string{*this, 45};
    DocumentItem<int>        n_draw_steps{*this, 150};
    DocumentItem<double> time_span_factor{*this, 1.5};
    DocumentItem<double> time_step_factor{*this, 0.5};
    DocumentItem<double>    sampling_rate{*this, 1e4};

    Settings(DocumentNode& parent): DocumentNode(parent)
    {

    }
};

static void to_json(json& obj, const Settings& value)
{
    to_json(obj["n_elements_limb"], value.n_elements_limb);
    to_json(obj["n_elements_string"], value.n_elements_string);
    to_json(obj["n_draw_steps"], value.n_draw_steps);
    to_json(obj["time_span_factor"], value.time_span_factor);
    to_json(obj["time_step_factor"], value.time_step_factor);
    to_json(obj["sampling_rate"], value.sampling_rate);
}

static void from_json(const json& obj, Settings& value)
{
    from_json(obj["n_elements_limb"], value.n_elements_limb);
    from_json(obj["n_elements_string"], value.n_elements_string);
    from_json(obj["n_draw_steps"], value.n_draw_steps);
    from_json(obj["time_span_factor"], value.time_span_factor);
    from_json(obj["time_step_factor"], value.time_step_factor);
    from_json(obj["sampling_rate"], value.sampling_rate);
}

