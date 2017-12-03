#pragma once
#include "bow/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Material: public DocumentNode
{
    DocumentItem<double>    rho{*this, 600.0};
    DocumentItem<double>      E{*this, 15e9};

    Material(DocumentNode& parent): DocumentNode(parent)
    {

    }
};

static void to_json(json& obj, const Material& value)
{
    to_json(obj["rho"], value.rho);
    to_json(obj["E"], value.E);
}

static void from_json(const json& obj, Material& value)
{
    from_json(obj["rho"], value.rho);
    from_json(obj["E"], value.E);
}

