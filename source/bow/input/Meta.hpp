#pragma once
#include "bow/document/Document.hpp"
#include "gui/Application.hpp"
#include <json.hpp>

using nlohmann::json;

struct Meta: public DocumentNode
{
    DocumentItem<std::string>  version{*this, Application::version};
    DocumentItem<std::string> comments{*this, ""};

    Meta(DocumentNode& parent): DocumentNode(parent)
    {

    }
};

static void to_json(json& obj, const Meta& value)
{
    to_json(obj["version"], value.version);
    to_json(obj["comments"], value.comments);
}

static void from_json(const json& obj, Meta& value)
{
    from_json(obj["version"], value.version);
    from_json(obj["comments"], value.comments);
}
