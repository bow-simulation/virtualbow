#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct String: public DocumentNode
{
    DocumentItem<double> strand_stiffness{*this, 3500.0};
    DocumentItem<double>   strand_density{*this, 0.0005};
    DocumentItem<double>        n_strands{*this, 12.0};

    void load(const json& obj)
    {
        strand_stiffness = (double) obj["strand_stiffness"];
        strand_density   = (double) obj["strand_density"];
        n_strands        = (double) obj["n_strands"];
    }

    void save(json& obj) const
    {
        obj["strand_stiffness"] = (double) strand_stiffness;
        obj["strand_density"]   = (double) strand_density;
        obj["n_strands"]        = (double) n_strands;
    }
};
