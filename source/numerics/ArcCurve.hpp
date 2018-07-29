#pragma once
#include "Series.hpp"
#include "Curve2D.hpp"

class ArcCurve
{
public:
    static Curve2D nodes(const Series& segments, double x0, double y0, double phi0);
    static Curve2D sample(const Series& segments, double x0, double y0, double phi0, unsigned n);
    static Curve2D sample(const Series& segments, double x0, double y0, double phi0, const std::vector<double>& s);

private:
    static Curve2D::Point eval_arc(Curve2D::Point p0, double kappa, double ds);
};

class ArcCurve2
{
public:
    ArcCurve2(const Series& segments, double x0, double y0, double phi0)
    {

    }

    // static Curve2D nodes(const Series& segments, double x0, double y0, double phi0);
    // static Curve2D sample(const Series& segments, double x0, double y0, double phi0, unsigned n);

private:
    static Curve2D::Point eval_arc(Curve2D::Point p0, double kappa, double ds);
};

