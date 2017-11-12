#pragma once
#include "model/document/Document.hpp"
#include "gui/Application.hpp"
#include <json.hpp>

using nlohmann::json;

struct Meta: public DocumentNode
{
    DocumentItem<std::string>  version{*this, validators::any<std::string>, Application::version};
    DocumentItem<std::string> comments{*this, validators::any<std::string>, ""};

    void load(const json& obj)
    {
        version.load(obj["version"]);
        comments.load(obj["comments"]);
    }

    void save(json& obj) const
    {
        version.save(obj["version"]);
        comments.save(obj["comments"]);
    }
};


