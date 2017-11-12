#pragma once
#include "model/document/Document.hpp"
#include "numerics/Series.hpp"
#include <json.hpp>

using nlohmann::json;

/*
struct Layer
{
    std::string name;
    Series height;
    double rho;
    double E;
};
*/

struct Sections: public DocumentNode
{
    DocumentItem<Series>  width{*this, validators::any<Series>, Series({0.0, 0.5, 1.0}, {0.04, 0.035, 0.01})};
    DocumentItem<Series> height{*this, validators::any<Series>, Series({0.0, 1.0}, {0.0128, 0.008})};
    DocumentItem<double>    rho{*this, validators::pos<double>, 600.0};
    DocumentItem<double>      E{*this, validators::pos<double>, 15e9};

    void load(const json& obj)
    {
        width.load(obj["width"]);
        height.load(obj["height"]);
        rho.load(obj["rho"]);
        E.load(obj["E"]);
    }

    void save(json& obj) const
    {
        width.save(obj["width"]);
        height.save(obj["height"]);
        rho.save(obj["rho"]);
        E.save(obj["E"]);
    }
};

