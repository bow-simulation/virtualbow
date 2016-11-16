#pragma once
#include "Series.hpp"
#include "Curve.hpp"

class ArcCurve
{
public:
    // n: Number of sampling steps. For n = 0 the segment nodes are calculated instead.
    // alpha: Angle between curve and y-axis
    static Curve sample(const Series& segments, double x0, double y0, double phi0, unsigned n)
    {
        if(n == 0)
        {
            Curve nodes;
            nodes.add_point({0.0, x0, y0, phi0});

            for(size_t i = 0; i < segments.size(); ++i)
            {
                auto p = eval_arc({nodes.s[i], nodes.x[i], nodes.y[i], nodes.phi[i]}, segments.val(i), segments.arg(i));
                nodes.add_point(p);
            }

            return nodes;
        }
        else
        {
            Curve curve;
            Curve nodes = ArcCurve::sample(segments, x0, y0, phi0, 0);
            curve.add_point({nodes.s[0], nodes.x[0], nodes.y[0], nodes.phi[0]});

            // i: Segment
            // j: Sampling interval
            unsigned i = 0;
            for(unsigned j = 0; j < n; ++j)
            {
                double sj = double(j+1)/double(n)*nodes.s.back();
                if(sj > nodes.s[i+1])
                {
                    ++i;
                }

                auto p = eval_arc({nodes.s[i], nodes.x[i], nodes.y[i], nodes.phi[i]}, segments.val(i), sj - nodes.s[i]);
                curve.add_point(p);
            }

            return curve;
        }
    }

private:
    // Calculates the end point of an arc segment with staring point p0, curvature kappa and arc length ds.
    static CurvePoint eval_arc(CurvePoint p0, double kappa, double ds)
    {
        CurvePoint p;

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
