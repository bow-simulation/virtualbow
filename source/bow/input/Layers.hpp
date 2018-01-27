#pragma once
#include "numerics/Series.hpp"
#include <json.hpp>

using nlohmann::json;

struct Layer
{
    std::string name = "unnamed";
    Series height = {{0.0, 1.0}, {0.0128, 0.008}};
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
    obj.at("name") = value.name;
    obj.at("height") = value.height;
    obj.at("rho") = value.rho;
    obj.at("E") = value.E;
}

static void from_json(const json& obj, Layer& value)
{
    value.name = obj.at("name");
    value.height = obj.at("height");
    value.rho = obj.at("rho");
    value.E = obj.at("E");
}
