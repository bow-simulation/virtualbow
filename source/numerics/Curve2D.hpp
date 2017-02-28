#pragma once
#include <vector>

struct Curve2D
{
    std::vector<double> s;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> phi;

    struct Point
    {
        double s;
        double x;
        double y;
        double phi;
    };

    void add_point(Point p)
    {
        s.push_back(p.s);
        x.push_back(p.x);
        y.push_back(p.y);
        phi.push_back(p.phi);
    }
};
