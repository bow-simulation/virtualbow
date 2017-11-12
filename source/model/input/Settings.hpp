#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Settings: public DocumentNode
{
    DocumentItem<int>     n_elements_limb{*this, validators::pos<int>, 40};
    DocumentItem<int>   n_elements_string{*this, validators::pos<int>, 45};
    DocumentItem<int>        n_draw_steps{*this, validators::pos<int>, 150};
    DocumentItem<double> time_span_factor{*this, validators::pos<double>, 1.5};
    DocumentItem<double> time_step_factor{*this, validators::pos<double>, 0.5};
    DocumentItem<double>    sampling_rate{*this, validators::pos<double>, 1e4};

    void load(const json& obj)
    {
        n_elements_limb.load(obj["n_elements_limb"]);
        n_elements_string.load(obj["n_elements_string"]);
        n_draw_steps.load(obj["n_draw_steps"]);
        time_span_factor.load(obj["time_span_factor"]);
        time_step_factor.load(obj["time_step_factor"]);
        sampling_rate.load(obj["sampling_rate"]);
    }

    void save(json& obj) const
    {
        n_elements_limb.save(obj["n_elements_limb"]);
        n_elements_string.save(obj["n_elements_string"]);
        n_draw_steps.save(obj["n_draw_steps"]);
        time_span_factor.save(obj["time_span_factor"]);
        time_step_factor.save(obj["time_step_factor"]);
        sampling_rate.save(obj["sampling_rate"]);
    }
};

