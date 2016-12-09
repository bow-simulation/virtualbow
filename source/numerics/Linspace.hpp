#pragma once
#include <boost/optional.hpp>

class Linspace
{
public:
    Linspace(double x0, double x1, size_t steps)
        : x0(x0), x1(x1), n(steps), i(0)
    {

    }

    Linspace()
    {

    }

    boost::optional<double> next()
    {
        if(i <= n)
        {
            double x = x0 + double(i)/double(n)*(x1 - x0);
            ++i;

            return x;
        }
        else
        {
            return boost::none;
        }
    }

private:
    double x0;
    double x1;
    size_t n;
    size_t i;
};
