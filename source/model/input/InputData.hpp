#pragma once
#include "model/document/Document.hpp"
#include "Meta.hpp"
#include "Profile.hpp"
#include "Sections.hpp"
#include "String.hpp"
#include "Masses.hpp"
#include "Operation.hpp"
#include "Settings.hpp"
#include <json.hpp>
#include <fstream>

using nlohmann::json;

struct InputData: public DocumentNode
{
    Meta           meta{*this};
    Profile     profile{*this};
    Sections   sections{*this};
    String       string{*this};
    Masses       masses{*this};
    Operation operation{*this};
    Settings   settings{*this};

    void load(const json& obj)
    {
        meta.load(obj["meta"]);
        profile.load(obj["profile"]);
        sections.load(obj["sections"]);
        string.load(obj["string"]);
        masses.load(obj["masses"]);
        operation.load(obj["operation"]);
        settings.load(obj["settings"]);
    }

    void save(json& obj) const
    {
        meta.save(obj["meta"]);
        profile.save(obj["profile"]);
        sections.save(obj["sections"]);
        string.save(obj["string"]);
        masses.save(obj["masses"]);
        operation.save(obj["operation"]);
        settings.save(obj["settings"]);
    }

    void load(const std::string& path)
    {
        std::ifstream stream(path);

        json obj;
        obj << stream;

        load(obj);
    }

    void save(const std::string& path)
    {
        json obj;
        save(obj);

        std::ofstream stream(path);
        stream << std::setw(4) << obj << std::endl;
    }
};
