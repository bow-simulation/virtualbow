#pragma once
#include "Eigen.hpp"
#include <functional>
#include <stdexcept>
#include <cassert>
#include <cmath>

class RootFinding
{
public:
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
    static double secant_method(const F& f, double x0, double x1, double epsilon, unsigned iter)
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

    // Regula falsi with Illinois improvement, adapted from https://en.wikipedia.org/wiki/Regula_falsi
    template<typename F>
    static double regula_falsi(const F& f, double xa, double xb, double fa, double fb, double ftol, double xtol, unsigned iter)
    {
        if(xa > xb)
            throw std::runtime_error("xb must be larger than xa");

        if(fa*fb > 0.0)
            throw std::runtime_error("fa and fb must have different sign");

       int side = 0;
       for(unsigned i = 0; i < iter; ++i)
       {
            double xc = (fa*xb - fb*xa)/(fa - fb);
            double fc = f(xc);

            // Check for convergence
            if(std::abs(fc) < ftol || std::abs(xb - xa) < xtol)
            {
                return xc;
            }

            if(fc*fb > 0.0)  // fb and fc have same sign, assign xc to xb
            {
                xb = xc;
                fb = fc;

                if(side == -1)
                    fa /= 2.0;

                side = -1;
            }
            else if(fa*fc > 0.0)  // fa and fc have same sign, assign xc to xa
            {
                xa = xc;
                fa = fc;

                if(side == 1)
                    fb /= 2.0;

                side = 1;
            }
            else  // fr*f_ exactly zero
            {
                return xc;
            }
        }
        throw std::runtime_error("Maximum number of iterations exceeded");
    }

    template<typename F>
    static double regula_falsi(const F& f, double xa, double xb, double ftol, double xtol, unsigned iter)
    {
        return regula_falsi(f, xa, xb, f(xa), f(xb), ftol, xtol, iter);
    }
};
