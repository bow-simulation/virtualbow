#pragma once
#include "Meta.hpp"
#include "Profile.hpp"
#include "Layer.hpp"
#include "String.hpp"
#include "Masses.hpp"
#include "Operation.hpp"
#include "Settings.hpp"
#include <json.hpp>
#include <vector>

using nlohmann::json;

struct InputData2
{
    Meta2 meta;
    Settings2 settings;
    Profile2 profile;
    Series width = {{0.0, 1.0}, {0.05, 0.01}};
    std::vector<Layer2> layers = {Layer2()};
    String2 string;
    Masses2 masses;
    Operation2 operation;
};

static void to_json(json& obj, const InputData2& value)
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

static void from_json(const json& obj, InputData2& value)
{
    value.meta = obj["meta"];
    value.settings = obj["settings"];
    value.profile = obj["profile"];
    value.width = obj["width"];
    value.layers = obj["layers"].get<std::vector<Layer2>>();
    value.string = obj["string"];
    value.masses = obj["masses"];
    value.operation = obj["operation"];
}
