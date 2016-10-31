#pragma once
#include <jsoncons/json.hpp>
#include <vector>

struct Series
{
    std::vector<double> args;
    std::vector<double> vals;
};

namespace jsoncons
{
    template<class Json>
    struct json_type_traits<Json, Series>
    {
        static Series as(const Json& rhs)
        {
            Series val;
            val.args = rhs["args"].template as<std::vector<double>>();
            val.vals = rhs["vals"].template as<std::vector<double>>();

            return val;
        }

        static Json to_json(const Series& val)
        {
            Json obj;
            obj["args"] = val.args;
            obj["vals"] = val.vals;

            return obj;
        }
    };
}
