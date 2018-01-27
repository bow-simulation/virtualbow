#pragma once
#include <string>
#include <json.hpp>

using nlohmann::json;

struct Meta
{
    std::string version = "";
    std::string comments = "";
};

static bool operator==(const Meta& lhs, const Meta& rhs)
{
    return lhs.version == rhs.version
        && lhs.comments == rhs.comments;
}

static bool operator!=(const Meta& lhs, const Meta& rhs)
{
    return !operator==(lhs, rhs);
}

static void to_json(json& obj, const Meta& value)
{
    obj.at("version") = value.version;
    obj.at("comments") = value.comments;
}

static void from_json(const json& obj, Meta& value)
{
    value.version = obj.at("version");
    value.comments = obj.at("comments");
}
