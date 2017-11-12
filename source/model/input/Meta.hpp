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
        version  = obj["version"].get<std::string>();
        comments = obj["comments"].get<std::string>();
    }

    void save(json& obj) const
    {
        obj["version"]  = (std::string) version;
        obj["comments"] = (std::string) comments;
    }
};


