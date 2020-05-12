#pragma once
#include "Settings.hpp"
#include "Layers.hpp"
#include "String.hpp"
#include "Masses.hpp"
#include "Damping.hpp"
#include "Dimensions.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>
#include <vector>

using nlohmann::json;

struct InputData
{
    std::string version = Config::APPLICATION_VERSION;
    std::string comment;
    Settings settings;
    Dimensions dimensions;
    Series profile = {{0.0, 0.8}, {0.0, 0.0}};
    Series width = {{0.0, 1.0}, {0.06, 0.01}};
    Layers layers = {Layer()};
    String string;
    Masses masses;
    Damping damping;

    InputData() = default;
    InputData(const std::string& path);
    void save(const std::string& path) const;
};

// Todo: Use "= default" in case it should make it into C++20
static bool operator==(const InputData& lhs, const InputData& rhs)
{
    return lhs.version == rhs.version
        && lhs.comment == rhs.comment
        && lhs.settings == rhs.settings
        && lhs.dimensions == rhs.dimensions
        && lhs.profile == rhs.profile
        && lhs.width == rhs.width
        && lhs.layers == rhs.layers
        && lhs.string == rhs.string
        && lhs.damping == rhs.damping
        && lhs.masses == rhs.masses;
}

static bool operator!=(const InputData& lhs, const InputData& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const InputData& value)
{
    obj["version"] = value.version;
    obj["comment"] = value.comment;
    obj["settings"] = value.settings;
    obj["dimensions"] = value.dimensions;
    obj["profile"] = value.profile;
    obj["width"] = value.width;
    obj["layers"] = value.layers;
    obj["string"] = value.string;
    obj["masses"] = value.masses;
    obj["damping"] = value.damping;
}

static void from_json(const json& obj, InputData& value)
{
    value.version = obj.at("version");
    value.comment = obj.at("comment");
    value.settings = obj.at("settings");
    value.dimensions = obj.at("dimensions");
    value.profile = obj.at("profile");
    value.width = obj.at("width");
    value.layers = obj.at("layers").get<Layers>();
    value.string = obj.at("string");
    value.masses = obj.at("masses");
    value.damping = obj.at("damping");
}
