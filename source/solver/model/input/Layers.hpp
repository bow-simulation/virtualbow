#pragma once
#include "solver/numerics/Series.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct Layer
{
    std::string name = "New layer";
    Series height = {{0.0, 1.0}, {0.015, 0.01}};
    double rho = 600.0;
    double E = 15e9;
};

using Layers = std::vector<Layer>;

static bool operator==(const Layer& lhs, const Layer& rhs)
{
    return lhs.name == rhs.name
        && lhs.height == rhs.height
        && lhs.rho == rhs.rho
        && lhs.E == rhs.E;
}

static bool operator!=(const Layer& lhs, const Layer& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Layer& value)
{
    obj["name"] = value.name;
    obj["height"] = value.height;
    obj["rho"] = value.rho;
    obj["E"] = value.E;
}

static void from_json(const json& obj, Layer& value)
{
    value.name = obj.at("name");
    value.height = obj.at("height");
    value.rho = obj.at("rho");
    value.E = obj.at("E");
}
