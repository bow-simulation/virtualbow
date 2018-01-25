#pragma once
#include <string>
#include <json.hpp>

using nlohmann::json;

struct Meta2
{
    std::string version = "";
    std::string comments = "";
};

static bool operator==(const Meta2& lhs, const Meta2& rhs)
{
    return lhs.version == rhs.version
        && lhs.comments == rhs.comments;
}

static void to_json(json& obj, const Meta2& value)
{
    obj["version"] = value.version;
    obj["comments"] = value.comments;
}

static void from_json(const json& obj, Meta2& value)
{
    value.version = obj["version"];
    value.comments = obj["comments"];
}
