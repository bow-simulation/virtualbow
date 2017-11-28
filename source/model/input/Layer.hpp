#pragma once
#include "model/document/Document.hpp"
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

    void load(const json& obj)
    {
        name   = obj["comments"].get<std::string>();
        height = (Series) obj["height"];
        rho    = (double) obj["rho"];
        E      = (double) obj["E"];
    }

    void save(json& obj) const
    {
        obj["name"]   = (std::string) name;
        obj["height"] = (Series)      height;
        obj["rho"]    = (double)      rho;
        obj["E"]      = (double)      E;
    }
};
