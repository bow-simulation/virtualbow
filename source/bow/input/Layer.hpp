#pragma once
#include "bow/document/Document.hpp"
#include "numerics/Series.hpp"
#include <json.hpp>

using nlohmann::json;

struct Layer: public DocumentNode
{
    DocumentItem<std::string> name{*this, ""};
    DocumentItem<Series>    height{*this, Series({0.0, 1.0}, {0.0128, 0.008})};
    DocumentItem<double>       rho{*this, 600.0};
    DocumentItem<double>         E{*this, 15e9};

    Layer(DocumentNode& parent): DocumentNode(parent)
    {

    }
};

static void to_json(json& obj, const Layer& value)
{
    to_json(obj["name"], value.name);
    to_json(obj["height"], value.height);
    to_json(obj["rho"], value.rho);
    to_json(obj["E"], value.E);
}

static void from_json(const json& obj, Layer& value)
{
    from_json(obj["name"], value.name);
    from_json(obj["height"], value.height);
    from_json(obj["rho"], value.rho);
    from_json(obj["E"], value.E);
}
