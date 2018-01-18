#pragma once
#include "Eigen.hpp"
#include <functional>
#include <stdexcept>
#include <cassert>
#include <cmath>

// Returns the real roots of the quadratic polynomial
// f(x) = c0 + c1*x + c2*x^2
// or NaN if no real solutions exist.
static Vector<2> solve_quadratic(double c0, double c1, double c2)
{
    double s = sqrt(c1*c1 - 4.0*c2*c0);
    return {(-c1 + s)/(2.0*c2), (-c1 - s)/(2.0*c2)};
}

// Find the root of the function f by using the secant method [1] with the initial values x0 and x1
// [1] https://en.wikipedia.org/wiki/Secant_method
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

    throw std::runtime_error("Secant method: Maximum number of iterations exceeded");
}

template<bool rising, class F>
double bisect(const F& f, double x_min, double x_max, double ftol, double xtol, unsigned iter)
{
    assert(x_min <= x_max);

    for(unsigned i = 0; i < iter; ++i)
    {
        double x_new = 0.5*(x_max + x_min);
        double f_new = f(x_new);

        if(std::abs(f_new) < ftol || x_max - x_min < xtol)
            return x_new;

        if((!rising && f_new > 0) || (rising && f_new < 0))
            x_min = x_new;
        else
            x_max = x_new;
    }

    throw std::runtime_error("Bisection: Maximum number of iterations exceeded");
}

template<class F>
double bracket_and_bisect(const F& f, double x0, double factor, double ftol, double xtol, unsigned int iter = 50)
{
    assert(x0 > 0.0);
    assert(factor > 1.0);

    // Root bracketing

    double xl = x0;
    double fl = f(xl);

    double xu = factor*x0;
    double fu = f(xu);

    for(unsigned int i = 0; i < iter; ++i)
    {
        if(fl*fu < 0)
            break;

        if(i == iter-1)
            throw std::runtime_error("Root bracketing: Maximum number of iterations exceeded");

        xl = xu;
        fl = fu;

        xu *= factor;
        fu = f(xu);
    }

    // Bisection

    for(unsigned int i = 0; i < iter; ++i)
    {
        double x_new = 0.5*(xl + xu);
        double f_new = f(x_new);

        if(std::abs(f_new) < ftol || xu - xl < xtol)
            return x_new;

        if(fl*f_new > 0)
        {
            xl = x_new;
            fl = f_new;
        }
        else
        {
            xu = x_new;
            fu = f_new;
        }
    }

    throw std::runtime_error("Bisection: Maximum number of iterations exceeded");
}
