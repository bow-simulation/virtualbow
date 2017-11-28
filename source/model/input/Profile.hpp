#pragma once
#include "model/document/Document.hpp"
#include "numerics/Series.hpp"
#include <json.hpp>

using nlohmann::json;

struct Profile: public DocumentNode
{
    DocumentItem<Series> segments{*this, Series({0.7}, {0.0})};
    DocumentItem<double>       x0{*this, 0.0};
    DocumentItem<double>       y0{*this, 0.0};
    DocumentItem<double>     phi0{*this, 0.0};

    Profile(DocumentNode& parent): DocumentNode(parent)
    {

    }
};

static void to_json(json& obj, const Profile& value)
{
    to_json(obj["segments"], value.segments);
    to_json(obj["x0"], value.x0);
    to_json(obj["y0"], value.y0);
    to_json(obj["phi0"], value.phi0);
}

static void from_json(const json& obj, Profile& value)
{
    from_json(obj["segments"], value.segments);
    from_json(obj["x0"], value.x0);
    from_json(obj["y0"], value.y0);
    from_json(obj["phi0"], value.phi0);
}
