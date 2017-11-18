#pragma once
#include "model/document/Document.hpp"
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

    Settings()
    {
        create_constraint(n_elements_limb, "Number of limb elements must be positive", &validators::pos<int>);
        create_constraint(n_elements_string, "Number of string elements must be positive", [](int x){ return x > 0; });
        create_constraint(n_draw_steps, "Number of draw steps must be positive", [](int x){ return x > 0; });
        create_constraint(time_span_factor, "Time span factor must be positive", [](double x){ return x > 0; });
        create_constraint(time_step_factor, "Time step factor must be between 0 and 1", [](double x){ return x > 0.0 && x < 1.0; });
        create_constraint(sampling_rate, "Sampling rate must be positive", [](double x){ return x > 0; });
    }

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

