#pragma once
#include "EigenTypes.hpp"
#include <cassert>
#include <cmath>

// f: double -> T
// xa: Lower bound
// xb: Upper bound

class AdaptiveSimpson
{
public:
    template<typename T, typename F>
    static T integrate(const F& f, double a, double b, double epsilon) {
        T fa = f(a);
        T fb = f(b);

        Triple<T> whole = simpson_quadrature(f, a, b, fa, fb);
        return simpson_quadrature_recursive(f, a, b, fa, fb, whole, epsilon);
    }

private:
    template<typename T>
    struct Triple {
        double m; T fm; T result;
    };

    template<typename T, typename F>
    static Triple<T> simpson_quadrature(const F& f, double a, double b, const T& fa, const T& fb) {
        double m = (a + b)/2.0;
        T fm = f(m);
        return {m, fm, std::abs(b - a)/6.0*(fa + 4.0*fm + fb)};
    }

    template<typename T, typename F>
    static T simpson_quadrature_recursive(const F& f, double a, double b, const T& fa, const T& fb, Triple<T> whole, double epsilon, int n = 15) {
        if(n == 0) {
            throw std::runtime_error("Maximum recursion depth reached");
        }

        Triple<T> left = simpson_quadrature(f, a, whole.m, fa, whole.fm);
        Triple<T> right = simpson_quadrature(f, whole.m, b, whole.fm, fb);
        T delta = left.result + right.result - whole.result;

        if(inf_norm(delta) <= 15.0*epsilon) {
            return left.result + right.result + delta/15.0;
        } else {
            return simpson_quadrature_recursive(f, a, whole.m, fa, whole.fm, left, epsilon/2.0, n-1)
                   + simpson_quadrature_recursive(f, whole.m, b, whole.fm, fb, right, epsilon/2.0, n-1);
        }
    }

    static double inf_norm(double delta) {
        return std::abs(delta);
    }

    template<typename Derived>
    static double inf_norm(const Eigen::MatrixBase<Derived>& delta) {
        return delta.template lpNorm<Eigen::Infinity>();
    }
};
