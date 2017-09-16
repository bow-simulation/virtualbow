#include "ArcCurve.hpp"
#include <cassert>

Curve2D ArcCurve::nodes(const Series& segments, double x0, double y0, double phi0)
{
    Curve2D nodes(segments.size() + 1);
    nodes.set_point(0, {0.0, x0, y0, phi0});

    for(size_t i = 0; i < segments.size(); ++i)
    {
        auto p = eval_arc({nodes.s[i], nodes.x[i], nodes.y[i], nodes.phi[i]}, segments.val(i), segments.arg(i));
        nodes.set_point(i+1, p);
    }
    
    return nodes;
}

// n: Number of sampling steps/intervals.
// alpha: Angle between curve and y-axis
Curve2D ArcCurve::sample(const Series& segments, double x0, double y0, double phi0, unsigned n)
{
    Curve2D nodes = ArcCurve::nodes(segments, x0, y0, phi0);

    unsigned j = 0; // Segment index
    auto eval_curve = [&](double s)
    {
        // Make sure that s[j] <= s <= s[j + 1]
        while(s > nodes.s[j+1])
            ++j;

        assert(s >= nodes.s[j]);

        // Todo: Add get_point(size_t) method to Curve2D?
        double kappa = segments.val(j);
        double ds = s - nodes.s[j];
        return eval_arc({nodes.s[j], nodes.x[j], nodes.y[j], nodes.phi[j]}, kappa, ds);
    };

    Curve2D result(n + 1);
    for(unsigned i = 0; i < n + 1; ++i)
    {
        double s = double(i)/double(n)*nodes.s.tail(1)(0);   // Todo: Better way for last element?
        result.set_point(i, eval_curve(s));
    }

    return result;
}

// Calculates the end point of an arc segment with staring point p0, curvature kappa and arc length ds.
Curve2D::Point ArcCurve::eval_arc(Curve2D::Point p0, double kappa, double ds)
{
    Curve2D::Point p;

    p.s = p0.s + ds;
    p.phi = p0.phi + kappa*ds;

    if(kappa == 0.0)
    {
        p.x = p0.x + cos(p0.phi)*ds;
        p.y = p0.y + sin(p0.phi)*ds;
    }
    else
    {
        p.x = p0.x + (sin(p.phi) - sin(p0.phi))/kappa;
        p.y = p0.y + (cos(p0.phi) - cos(p.phi))/kappa;
    }

    return p;
}
