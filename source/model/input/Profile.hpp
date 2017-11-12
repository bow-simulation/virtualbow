#pragma once
#include "model/document/Document.hpp"
#include "numerics/Series.hpp"
#include <json.hpp>

using nlohmann::json;

struct Profile: public DocumentNode
{
    DocumentItem<Series> segments{*this, validators::any<Series>, Series({0.7}, {0.0})};
    DocumentItem<double>       x0{*this, validators::any<double>, 0.0};
    DocumentItem<double>       y0{*this, validators::any<double>, 0.0};
    DocumentItem<double>     phi0{*this, validators::any<double>, 0.0};

    void load(const json& obj)
    {
        segments.load(obj["segments"]);
        x0.load(obj["x0"]);
        y0.load(obj["y0"]);
        phi0.load(obj["phi0"]);
    }

    void save(json& obj) const
    {
        segments.save(obj["segments"]);
        x0.save(obj["x0"]);
        y0.save(obj["y0"]);
        phi0.save(obj["phi0"]);
    }
};
