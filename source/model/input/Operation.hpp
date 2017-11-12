#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Operation: public DocumentNode
{
    DocumentItem<double> brace_height{*this, validators::any<double>, 0.2};
    DocumentItem<double>  draw_length{*this, validators::any<double>, 0.7};
    DocumentItem<double>   mass_arrow{*this, validators::pos<double>, 0.025};

    void load(const json& obj)
    {
        brace_height.load(obj["brace_height"]);
        draw_length.load(obj["draw_length"]);
        mass_arrow.load(obj["mass_arrow"]);
    }

    void save(json& obj) const
    {
        brace_height.save(obj["brace_height"]);
        draw_length.save(obj["draw_length"]);
        mass_arrow.save(obj["mass_arrow"]);
    }
};

