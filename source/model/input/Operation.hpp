#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Operation: public DocumentNode
{
    DocumentItem<double> brace_height{*this, 0.2};
    DocumentItem<double>  draw_length{*this, 0.7};
    DocumentItem<double>   mass_arrow{*this, 0.025};

    Operation(DocumentNode& parent): DocumentNode(parent)
    {

    }
};

static void to_json(json& obj, const Operation& value)
{
    to_json(obj["brace_height"], value.brace_height);
    to_json(obj["draw_length"], value.draw_length);
    to_json(obj["mass_arrow"], value.mass_arrow);
}

static void from_json(const json& obj, Operation& value)
{
    from_json(obj["brace_height"], value.brace_height);
    from_json(obj["draw_length"], value.draw_length);
    from_json(obj["mass_arrow"], value.mass_arrow);
}

