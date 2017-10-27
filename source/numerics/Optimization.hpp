#pragma once
#include <stdexcept>
#include <cmath>

// Golden section search for minimizing a one-dimensional unimodal function f in the interval [xa, xb]
// Implementation based on https://en.wikipedia.org/wiki/Golden-section_search

template<class F>
double golden_section_search(const F& f, double xa, double xb, double xtol, unsigned iter)
{
    const double ratio = (sqrt(5) + 1)/2;

    double xc = xb - (xb - xa)/ratio;
    double xd = xa + (xb - xa)/ratio;

    for(unsigned i = 0; i < iter; ++i)
    {
        if(std::abs(xc - xd) < xtol)
            return (xb + xa)/2;

        if(f(xc) < f(xd))
            xb = xd;
        else
            xa = xc;

        xc = xb - (xb - xa)/ratio;
        xd = xa + (xb - xa)/ratio;
    }

    throw std::runtime_error("Golden section search: Maximum number of iterations exceeded");
}
