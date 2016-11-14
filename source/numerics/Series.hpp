#pragma once
#include <jsoncons/json.hpp>
#include <vector>

class Series
{
public:
    Series()
    {

    }

    Series(std::vector<double> args, std::vector<double> vals)  // Todo: Is this overhead? Maybe take initializer lists?
        : m_args(args), m_vals(vals)
    {
        if(args.size() != vals.size())
            throw std::runtime_error("Arguments and values must have the same size");
    }

    void push_back(double arg, double val)
    {
        m_args.push_back(arg);
        m_vals.push_back(val);
    }

    void insert(size_t i, double arg, double val)
    {
        m_args.insert(m_args.begin() + i, arg);
        m_vals.insert(m_vals.begin() + i, arg);
    }

    void remove(size_t i)
    {
        // Todo: Seriously?
        m_args.erase(m_args.begin() + i);
        m_vals.erase(m_vals.begin() + i);
    }

    void remove()
    {
        if(size() > 0)
        {
            m_args.pop_back();
            m_vals.pop_back();
        }
    }

    const double& arg(size_t i) const
    {
        return m_args[i];
    }

    const double& val(size_t i) const
    {
        return m_vals[i];
    }

    double& arg(size_t i)
    {
        return m_args[i];
    }

    double& val(size_t i)
    {
        return m_vals[i];
    }

    const std::vector<double>& args() const
    {
        return m_args;
    }

    const std::vector<double>& vals() const
    {
        return m_vals;
    }

    size_t size() const
    {
        return m_args.size();
    }

    bool operator==(const Series& rhs) const
    {
        return m_args == rhs.m_args && m_vals == rhs.m_vals;
    }

    bool operator!=(const Series& rhs) const
    {
        return !operator==(rhs);
    }

    Series flip(bool vertical)  // Todo: Use enum instead of bool and a better name
    {

        // Todo: Remove code duplication
        if(vertical)
        {
            std::vector<double> new_vals;
            std::for_each(m_vals.begin(), m_vals.end(), [&](double val){ new_vals.push_back(-val); });
            return Series(m_args, new_vals);
        }
        else
        {
            std::vector<double> new_args;
            std::for_each(m_args.begin(), m_args.end(), [&](double arg){ new_args.push_back(-arg); });
            return Series(new_args, m_vals);
        }
    }

private:
    std::vector<double> m_args;
    std::vector<double> m_vals;

    template<typename> friend class jsoncons::json_type_traits;
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
