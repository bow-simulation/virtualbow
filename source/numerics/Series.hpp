#pragma once
#include <vector>
#include <json.hpp>

using nlohmann::json;

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

// Todo //private:
    std::vector<double> m_args;
    std::vector<double> m_vals;

    //friend void to_json(json&, const Series&);
    //friend void from_json(const json&, Series&);
};

static void to_json(json& obj, const Series& val)
{
    obj.push_back(val.m_args);
    obj.push_back(val.m_vals);
}

static void from_json(const json& obj, Series& val)
{
    val.m_args = obj.at(0).get<std::vector<double>>();
    val.m_vals = obj.at(1).get<std::vector<double>>();
}
