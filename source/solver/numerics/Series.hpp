#pragma once
#include <vector>

class Series
{
public:
    Series() = default;
    Series(std::vector<double> args, std::vector<double> vals);

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

private:
    std::vector<double> m_args;
    std::vector<double> m_vals;
};
