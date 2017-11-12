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

using nlohmann::json;

#include <QtCore>

struct InputData: public DocumentNode
{
    Meta meta;
    Profile profile;
    Sections sections;
    String string;
    Masses masses;
    Operation operation;
    Settings settings;

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

    void load(const std::string& path);
    void save(const std::string& path);
};
