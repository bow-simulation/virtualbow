#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Material: public DocumentNode
{
    DocumentItem<double>    rho{*this, 600.0};
    DocumentItem<double>      E{*this, 15e9};

    Material(DocumentNode& parent): DocumentNode(parent)
    {
        create_constraint(rho, "Density must be positive",         [](double x){ return x > 0; });
        create_constraint(E,   "Elastic modulus must be positive", [](double x){ return x > 0; });
    }

    void load(const json& obj)
    {
        rho    = (double) obj["rho"];
        E      = (double) obj["E"];
    }

    void save(json& obj) const
    {
        obj["rho"]    = (double) rho;
        obj["E"]      = (double) E;
    }
};


