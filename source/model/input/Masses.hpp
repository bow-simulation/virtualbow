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

    }
};

static void to_json(json& obj, const Masses& value)
{
    to_json(obj["string_center"], value.string_center);
    to_json(obj["string_tip"], value.string_tip);
    to_json(obj["limb_tip"], value.limb_tip);
}

static void from_json(const json& obj, Masses& value)
{
    from_json(obj["string_center"], value.string_center);
    from_json(obj["string_tip"], value.string_tip);
    from_json(obj["limb_tip"], value.limb_tip);
}

