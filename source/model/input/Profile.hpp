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

    Profile()
    {
        /*
        create_constraint(rho, "Density must be positive",         [](double x){ return x > 0; });
        create_constraint(E,   "Elastic modulus must be positive", [](double x){ return x > 0; });
        */
    }

    void load(const json& obj)
    {
        segments = (Series) obj["segments"];
        x0       = (double) obj["x0"];
        y0       = (double) obj["y0"];
        phi0     = (double) obj["phi0"];
    }

    void save(json& obj) const
    {
        obj["segments"] = (Series) segments;
        obj["x0"]       = (double) x0;
        obj["y0"]       = (double) y0;
        obj["phi0"]     = (double) phi0;
    }
};
