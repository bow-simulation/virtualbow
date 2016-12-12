#pragma once
#include <boost/optional.hpp>

template<typename T>
class Linspace
{
public:
    Linspace(T x0, T x1, size_t steps)
        : x0(x0), x1(x1), n(steps), i(0)
    {

    }

    Linspace()
    {

    }

    boost::optional<T> next()
    {
        if(i <= n)
        {
            T x = x0 + T(i)/T(n)*(x1 - x0);
            ++i;

            return x;
        }
        else
        {
            return boost::none;
        }
    }

private:
    T x0;
    T x1;
    size_t n;
    size_t i;
};
