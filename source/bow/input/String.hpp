#pragma once
#include "bow/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct String: public DocumentNode
{
    DocumentItem<double> strand_stiffness{*this, 3500.0};
    DocumentItem<double>   strand_density{*this, 0.0005};
    DocumentItem<double>        n_strands{*this, 12.0};

    String(DocumentNode& parent): DocumentNode(parent)
    {

    }
};

static void to_json(json& obj, const String& value)
{
    to_json(obj["strand_stiffness"], value.strand_stiffness);
    to_json(obj["strand_density"], value.strand_density);
    to_json(obj["n_strands"], value.n_strands);
}

static void from_json(const json& obj, String& value)
{
    from_json(obj["strand_stiffness"], value.strand_stiffness);
    from_json(obj["strand_density"], value.strand_density);
    from_json(obj["n_strands"], value.n_strands);
}
