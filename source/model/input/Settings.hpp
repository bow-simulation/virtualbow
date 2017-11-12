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
        n_elements_limb   = (int)    obj["n_elements_limb"];
        n_elements_string = (int)    obj["n_elements_string"];
        n_draw_steps      = (int)    obj["n_draw_steps"];
        time_span_factor  = (double) obj["time_span_factor"];
        time_step_factor  = (double) obj["time_step_factor"];
        sampling_rate     = (double) obj["sampling_rate"];
    }

    void save(json& obj) const
    {
        obj["n_elements_limb"]   = (int)    n_elements_limb;
        obj["n_elements_string"] = (int)    n_elements_string;
        obj["n_draw_steps"]      = (int)    n_draw_steps;
        obj["time_span_factor"]  = (double) time_span_factor;
        obj["time_step_factor"]  = (double) time_step_factor;
        obj["sampling_rate"]     = (double) sampling_rate;
    }
};

