#include "Series.hpp"

Series::Series()
{

}

Series::Series(std::vector<double> args, std::vector<double> vals)  // Todo: Is this overhead? Maybe take initializer lists?
    : m_args(args), m_vals(vals)
{
    if(args.size() != vals.size())
        throw std::runtime_error("Arguments and values must have the same size");
}

void Series::push_back(double arg, double val)
{
    m_args.push_back(arg);
    m_vals.push_back(val);
}

void Series::insert(size_t i, double arg, double val)
{
    m_args.insert(m_args.begin() + i, arg);
    m_vals.insert(m_vals.begin() + i, arg);
}

void Series::remove(size_t i)
{
    // Todo: Seriously?
    m_args.erase(m_args.begin() + i);
    m_vals.erase(m_vals.begin() + i);
}

void Series::remove()
{
    if(size() > 0)
    {
        m_args.pop_back();
        m_vals.pop_back();
    }
}

const double& Series::arg(size_t i) const
{
    return m_args[i];
}

const double& Series::val(size_t i) const
{
    return m_vals[i];
}

double& Series::arg(size_t i)
{
    return m_args[i];
}

double& Series::val(size_t i)
{
    return m_vals[i];
}

const std::vector<double>& Series::args() const
{
    return m_args;
}

const std::vector<double>& Series::vals() const
{
    return m_vals;
}

size_t Series::size() const
{
    return m_args.size();
}

bool Series::operator==(const Series& rhs) const
{
    return m_args == rhs.m_args && m_vals == rhs.m_vals;
}

bool Series::operator!=(const Series& rhs) const
{
    return !operator==(rhs);
}

Series Series::flip(bool vertical)  // Todo: Use enum instead of bool and a better name
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
