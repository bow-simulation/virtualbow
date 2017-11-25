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


