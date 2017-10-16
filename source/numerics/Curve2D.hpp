#pragma once
#include "numerics/Eigen.hpp"

struct Curve2D
{
    VectorXd s;
    VectorXd x;
    VectorXd y;
    VectorXd phi;

    Curve2D(size_t n)
        : s(n), x(n), y(n), phi(n)
    {

    }

    struct Point
    {
        double s;
        double x;
        double y;
        double phi;
    };

    void set_point(size_t i, Point p)
    {
        s[i] = p.s;
        x[i] = p.x;
        y[i] = p.y;
        phi[i] = p.phi;
    }
};
