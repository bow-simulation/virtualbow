#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Masses: public DocumentNode
{
    DocumentItem<double> string_center{*this, 0.005};
    DocumentItem<double>    string_tip{*this, 0.005};
    DocumentItem<double>      limb_tip{*this, 0.005};

    Masses(DocumentNode& parent): DocumentNode(parent)
    {
        create_constraint(string_center, "String center mass must be positive", [](double x){ return x > 0; });
        create_constraint(string_tip,    "String tip mass must be positive",    [](double x){ return x > 0; });
        create_constraint(limb_tip,      "Limb tip mass must be positive",      [](double x){ return x > 0; });
    }

    void load(const json& obj)
    {
        string_center = (double) obj["string_center"];
        string_tip    = (double) obj["string_tip"];
        limb_tip      = (double) obj["limb_tip"];
    }

    void save(json& obj) const
    {
        obj["string_center"] = (double) string_center;
        obj["string_tip"]    = (double) string_tip;
        obj["limb_tip"]      = (double) limb_tip;
    }
};
