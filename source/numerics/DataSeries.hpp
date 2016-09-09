#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <vector>
#include <cassert>

class DataSeries
{
public:
    DataSeries()
    {

    }

    DataSeries(std::vector<double> args, std::vector<double> vals)
        : m_args(args), m_vals(vals)
    {
        if(args.size() != vals.size())
            throw std::runtime_error("Arguments and values must have the same size");
    }

    void add(double arg, double val)
    {
        m_args.push_back(arg);
        m_vals.push_back(val);
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

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(cereal::make_nvp("args", m_args),
                cereal::make_nvp("vals", m_vals));
    }

private:
    std::vector<double> m_args;
    std::vector<double> m_vals;
};
