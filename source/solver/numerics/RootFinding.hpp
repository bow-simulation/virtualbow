#pragma once
#include "EigenTypes.hpp"
#include <Eigen/LU>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <cmath>

// Returns the real roots of the quadratic polynomial
// f(x) = c0 + c1*x + c2*x^2
// or NaN if no real solutions exist.
inline Vector<2> solve_quadratic(double c0, double c1, double c2) {
    double s = sqrt(c1*c1 - 4.0*c2*c0);
    return {(-c1 + s)/(2.0*c2), (-c1 - s)/(2.0*c2)};
}

// Find the root of the function f by using the secant method [1] with the initial values x0 and x1
// [1] https://en.wikipedia.org/wiki/Secant_method
template<class F>
inline double secant_method(const F& f, double x0, double x1, double ftol, unsigned iter) {
    double f0 = f(x0);
    double f1 = f(x1);

    for(unsigned i = 0; i < iter; ++i) {
        if(std::abs(f1) < ftol) {
            return x1;
        }

        double x2 = x1 - (x1 - x0)/(f1 - f0)*f1;

        x0 = x1; f0 = f1;
        x1 = x2; f1 = f(x2);
    }

    throw std::runtime_error("Secant method: Maximum number of iterations exceeded");
}

template<bool rising, class F>
inline double bisect(const F& f, double x_min, double x_max, double ftol, double xtol, unsigned iter) {
    assert(x_min <= x_max);

    for(unsigned i = 0; i < iter; ++i) {
        double x_new = 0.5*(x_max + x_min);
        double f_new = f(x_new);

        if(std::abs(f_new) < ftol || x_max - x_min < xtol) {
            return x_new;
        }

        if((!rising && f_new > 0) || (rising && f_new < 0)) {
            x_min = x_new;
        }
        else {
            x_max = x_new;
        }
    }

    throw std::runtime_error("Bisection: Maximum number of iterations exceeded");
}

// Numerical derivative of the function f: Vector<N> -> Vector<N> at point x0 with step size h
template<typename F, unsigned N>
Matrix<N> differentiate(const F& f, const Vector<N>& x0, double h) {
    Matrix<N> D = Matrix<N>::Zero();
    for(unsigned i = 0; i < N; ++i) {
        Vector<N> delta = h*Vector<N>::Unit(i);
        D.col(i) = (f(x0 + delta) - f(x0 - delta))/(2.0*h);
    }

    return D;
}

// Newton method for multi-dimensional, small fixed-size problems.
// Uses numerical derivatives with step size h.
//
// Each iteration, the function value is compared to ftol (maximum norm)
// and the step in x is compared to xtol (also maximum norm)
template<typename F, unsigned N>
Vector<3> newton_method(const F& f, Vector<N> x, double h, double xtol, double ftol, unsigned iter) {
    for(int i = 0; i < iter; ++i) {
        Vector<N> fi = f(x);
        if(fi.template lpNorm<Eigen::Infinity>() <= ftol) {
            return x;
        }

        Matrix<N> Ji = differentiate<F, N>(f, x, h);
        Vector<N> dx = -Ji.inverse()*f(x);
        if(dx.template lpNorm<Eigen::Infinity>() <= xtol) {
            return x;
        }

        x += dx;
    }

    throw std::runtime_error("No convergence");
}
