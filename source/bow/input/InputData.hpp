#pragma once
#include "Meta.hpp"
#include "Profile.hpp"
#include "Layers.hpp"
#include "String.hpp"
#include "Masses.hpp"
#include "Operation.hpp"
#include "Settings.hpp"
#include <json.hpp>
#include <vector>

using nlohmann::json;

struct InputData
{
    Meta meta;
    Settings settings;
    Profile profile;
    Series width = {{0.0, 1.0}, {0.06, 0.01}};
    Layers layers = {Layer()};
    String string;
    Masses masses;
    Operation operation;

    // Todo: Move those somewhere else, maybe in Application class. Interface of InputData can be just json.
    void load(const std::string& path);
    void save(const std::string& path) const;
};

// Todo: Use "= default" in case it should make it into C++20
static bool operator==(const InputData& lhs, const InputData& rhs)
{
    return lhs.meta == rhs.meta
        && lhs.settings == rhs.settings
        && lhs.profile == rhs.profile
        && lhs.width == rhs.width
        && lhs.layers == rhs.layers
        && lhs.string == rhs.string
        && lhs.masses == rhs.masses
        && lhs.operation == rhs.operation;
}

static bool operator!=(const InputData& lhs, const InputData& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const InputData& value)
{
    obj["meta"] = value.meta;
    obj["settings"] = value.settings;
    obj["profile"] = value.profile;
    obj["width"] = value.width;
    obj["layers"] = value.layers;
    obj["string"] = value.string;
    obj["masses"] = value.masses;
    obj["operation"] = value.operation;
}

static void from_json(const json& obj, InputData& value)
{
    value.meta = obj.at("meta");
    value.settings = obj.at("settings");
    value.profile = obj.at("profile");
    value.width = obj.at("width");
    value.layers = obj.at("layers").get<Layers>();
    value.string = obj.at("string");
    value.masses = obj.at("masses");
    value.operation = obj.at("operation");
}
