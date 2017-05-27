#pragma once
#include <jsoncons/json.hpp>
#include <vector>

class Series
{
public:
    Series();
    Series(std::vector<double> args, std::vector<double> vals);  // Todo: Is this overhead? Maybe take initializer lists?

    void push_back(double arg, double val);
    void insert(size_t i, double arg, double val);
    void remove(size_t i);
    void remove();

    const double& arg(size_t i) const;
    const double& val(size_t i) const;
    double& arg(size_t i);
    double& val(size_t i);


    const std::vector<double>& args() const;
    const std::vector<double>& vals() const;

    size_t size() const;

    bool operator==(const Series& rhs) const;
    bool operator!=(const Series& rhs) const;

    Series flip(bool vertical);  // Todo: Use enum instead of bool and a better name


//private:
    std::vector<double> m_args;
    std::vector<double> m_vals;

    //template<typename> friend class jsoncons::json_type_traits;
};

namespace jsoncons
{
    template<class Json>
    struct json_type_traits<Json, Series>
    {
        static Series as(const Json& rhs)
        {
            Series series;
            series.m_args = rhs["args"].template as<std::vector<double>>();
            series.m_vals = rhs["vals"].template as<std::vector<double>>();

            return series;
        }

        static Json to_json(const Series& series)
        {
            Json obj;
            obj["args"] = series.m_args;
            obj["vals"] = series.m_vals;

            return obj;
        }
    };
}
