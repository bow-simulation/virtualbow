#pragma once
#include "Series.hpp"

#include <vector>
#include <stdexcept>
#include <algorithm>

// Cubic spline that preserves monotonicity of the input data.
// Implementation adapted from https://en.wikipedia.org/wiki/Monotone_cubic_interpolation

class CubicSpline
{
public:
    CubicSpline(Series data)
        : xs(data.args()),
          ys(data.vals())
    {
        if(data.size() < 2)
            throw std::runtime_error("at least two data points are needed");

        if(!is_strictly_increasing(xs))
            throw std::runtime_error("function arguments must be strictly increasing");

        // Get consecutive differences and slopes
        std::vector<double> dys;
        std::vector<double> dxs;
        std::vector<double> ms;

        size_t length = xs.size();

        for(size_t i = 0; i < length-1; ++i)
        {
            double dx = xs[i+1] - xs[i];
            double dy = ys[i+1] - ys[i];

            dxs.push_back(dx);
            dys.push_back(dy);
            ms.push_back(dy/dx);
        }

        // Get degree-1 coefficients
        c1s.push_back(ms[0]);
        for(size_t i = 0; i < dxs.size() - 1; ++i)
        {
            if (ms[i]*ms[i+1] <= 0.0)
            {
                c1s.push_back(0.0);
            }
            else
            {
                double common = dxs[i] + dxs[i+1];
                c1s.push_back(3*common/((common + dxs[i+1])/ms[i] + (common + dxs[i])/ms[i+1]));
            }
        }
        c1s.push_back(ms.back());

        // Get degree-2 and degree-3 coefficients
        for(size_t i = 0; i < c1s.size() - 1; ++i)
        {
            double common = c1s[i] + c1s[i+1] - 2.0*ms[i];
            c2s.push_back((ms[i] - c1s[i] - common)/dxs[i]);
            c3s.push_back(common/(dxs[i]*dxs[i]));
        }
    }

    Series sample(size_t n)
    {
        double x0 = xs.front();
        double x1 = xs.back();

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
            while(arg > xs[j+1])    // Advance segment index such that x[j] < arg < x[j + 1]
                ++j;

            double h = arg - xs[j];
            return c3s[j]*h*h*h + c2s[j]*h*h + c1s[j]*h + ys[j];
        };

        for(size_t i = 0; i < args.size(); ++i)
            vals[i] = eval_ascending(args[i]);

        return vals;
    }

    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<double> c1s;
    std::vector<double> c2s;
    std::vector<double> c3s;
};
