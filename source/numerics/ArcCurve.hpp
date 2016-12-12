#pragma once
#include "Series.hpp"
#include "Curve.hpp"

class ArcCurve
{
public:
    static Curve sample(const Series& segments, double x0, double y0, double phi0, unsigned n);

private:
    static CurvePoint eval_arc(CurvePoint p0, double kappa, double ds);
};
