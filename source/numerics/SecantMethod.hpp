#pragma once
#include <functional>
#include <stdexcept>

// Find the root of the function f by using the secant method [1] with the initial values xa, xb
// [1] https://en.wikipedia.org/wiki/Secant_method
// Todo: Can the lambda be called statically?
template<class F>
double secant_method(const F& f, double x0, double x1, double epsilon, unsigned iter)
{
    double f0 = f(x0);
    double f1 = f(x1);

    for(unsigned i = 0; i < iter; ++i)
    {
        if(std::abs(f1) < epsilon)
        {
            return x1;
        }

        double x2 = x1 - (x1 - x0)/(f1 - f0)*f1;

        x0 = x1; f0 = f1;
        x1 = x2; f1 = f(x2);
    }

    throw std::runtime_error("Maximum number of iterations exceeded");
}
