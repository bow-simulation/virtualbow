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
    DocumentItem<Series>  width{*this, Series({0.0, 0.5, 1.0}, {0.04, 0.035, 0.01})};
    DocumentItem<Series> height{*this, Series({0.0, 1.0}, {0.0128, 0.008})};
    DocumentItem<double>    rho{*this, 600.0};
    DocumentItem<double>      E{*this, 15e9};

    Sections(DocumentNode& parent): DocumentNode(parent)
    {
        create_constraint(rho, "Density must be positive",         [](double x){ return x > 0; });
        create_constraint(E,   "Elastic modulus must be positive", [](double x){ return x > 0; });
    }

    void load(const json& obj)
    {
        width  = (Series) obj["width"];
        height = (Series) obj["height"];
        rho    = (double) obj["rho"];
        E      = (double) obj["E"];
    }

    void save(json& obj) const
    {
        obj["width"]  = (Series) width;
        obj["height"] = (Series) height;
        obj["rho"]    = (double) rho;
        obj["E"]      = (double) E;
    }
};

