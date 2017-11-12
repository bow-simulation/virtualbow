#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Masses: public DocumentNode
{
    DocumentItem<double> string_center{*this, validators::non_neg<double>, 0.005};
    DocumentItem<double>    string_tip{*this, validators::non_neg<double>, 0.005};
    DocumentItem<double>      limb_tip{*this, validators::non_neg<double>, 0.005};

    void load(const json& obj)
    {
        string_center.load(obj["string_center"]);
        string_tip.load(obj["string_tip"]);
        limb_tip.load(obj["limb_tip"]);
    }

    void save(json& obj) const
    {
        string_center.save(obj["string_center"]);
        string_tip.save(obj["string_tip"]);
        limb_tip.save(obj["limb_tip"]);
    }
};
