#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct String: public DocumentNode
{
    DocumentItem<double> strand_stiffness{*this, validators::pos<double>, 3500.0};
    DocumentItem<double>   strand_density{*this, validators::pos<double>, 0.0005};
    DocumentItem<double>        n_strands{*this, validators::pos<double>, 12.0};

    void load(const json& obj)
    {
        strand_stiffness.load(obj["strand_stiffness"]);
        strand_density.load(obj["strand_density"]);
        n_strands.load(obj["n_strands"]);
    }

    void save(json& obj) const
    {
        strand_stiffness.save(obj["strand_stiffness"]);
        strand_density.save(obj["strand_density"]);
        n_strands.save(obj["n_strands"]);
    }
};
