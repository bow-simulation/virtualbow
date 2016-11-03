#pragma once
#include "Series.hpp"
#include <vector>
#include <cmath>

struct ArcCurve
{
    std::vector<double> s;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> phi;

    // n: Number of points. For n = 0 the segment nodes are calculated.
    // Angle: Angle between curve and y-axis
    ArcCurve(const Series& segments, double x0, double y0, double angle, unsigned n)
    {
        if(n == 0)
        {
            add_point({0.0, x0, y0, angle +  M_PI_2});

            for(size_t i = 0; i < segments.size(); ++i)
            {
                auto p = eval_arc({s[i], x[i], y[i], phi[i]}, segments.val(i), segments.arg(i));
                add_point(p);
            }
        }
        else
        {
            ArcCurve nodes(segments, x0, y0, angle, 0);
            add_point({nodes.s[0], nodes.x[0], nodes.y[0], nodes.phi[0]});

            unsigned i = 0;
            for(unsigned j = 0; j < n; ++j)
            {
                double sj = double(j)/double(n-1)*nodes.s.back();
                if(sj > nodes.s[i + 1])
                {
                    ++i;
                }

                auto p = eval_arc({nodes.s[i], nodes.x[i], nodes.y[i], nodes.phi[i]}, segments.val(i), sj - nodes.s[i]);
                add_point(p);
            }
        }
    }
private:
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

    Point eval_arc(Point p0, double kappa, double ds) const
    {
        Point p;

        p.s = p0.s + ds;
        p.phi = p0.phi + kappa*ds;

        if(kappa == 0.0)
        {
            p.x = p0.x + std::cos(p0.phi)*ds;
            p.y = p0.y + std::sin(p0.phi)*ds;
        }
        else
        {
            p.x = p0.x + (std::sin(p.phi) - std::sin(p0.phi))/kappa;
            p.y = p0.y + (std::cos(p0.phi) - std::cos(p.phi))/kappa;
        }

        return p;
    }
};
