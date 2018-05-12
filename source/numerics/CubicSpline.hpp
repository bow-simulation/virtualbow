#pragma once
#include "Series.hpp"

#include <vector>
#include <stdexcept>
#include <algorithm>

class CubicSpline
{
public:
    CubicSpline(Series data)
        : x(data.args()), y(data.vals())
    {
        if(data.size() < 2)
            throw std::runtime_error("at least two data points are needed");

        if(!is_strictly_increasing(x))
            throw std::runtime_error("function arguments must be strictly increasing");

        init_abc_linear();
    }

    Series sample(size_t n)
    {
        double x0 = x.front();
        double x1 = x.back();

        Series result;

        std::vector<double> args;
        args.resize(n+1);

        for(size_t i = 0; i <= n; ++i)
        {
            double p = double(i)/double(n);
            args[i] = x0*(1.0 - p) + x1*p;
        }

        return Series(args, interpolate(args));
    }

private:
    void init_abc_linear()
    {
        size_t n = x.size();

        a.resize(n-1);
        b.resize(n-1);
        c.resize(n-1);

        for(size_t i = 0; i < n-1; ++i)
        {
            a[i] = 0.0;
            b[i] = 0.0;
            c[i] = (y[i+1] - y[i])/(x[i+1] - x[i]);
        }
    }

    bool is_strictly_increasing(const std::vector<double>& args)
    {
        for(size_t i = 0; i < args.size()-1; ++i)
        {
            if(args[i] >= args[i+1])
                return false;
        }

        return true;
    }

    std::vector<double> interpolate(const std::vector<double>& args)
    {
        if(!is_strictly_increasing(args))
            throw std::runtime_error("function arguments must be strictly increasing");

        std::vector<double> vals;
        vals.resize(args.size());

        size_t j = 0; // Last segment index
        auto eval_ascending = [&](double arg)
        {
            while(arg > x[j+1])    // Advance segment index such that x[j] < arg < x[j + 1]
                ++j;

            double h = arg - x[j];
            return a[j]*h*h*h + b[j]*h*h + c[j]*h + y[j];
        };

        for(size_t i = 0; i < args.size(); ++i)
            vals[i] = eval_ascending(args[i]);

        return vals;
    }

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> c;
};
