#include "ProfileCurve2.hpp"
#include "solver/numerics/Integration.hpp"
#include "solver/numerics/FindInterval.hpp"
#include <nlopt.hpp>
#include <stdexcept>
#include <cmath>
#include <utility>
#include <iostream>


Segment::Segment(const Point& p, const std::vector<double>& c)
    : p(p), c(c)
{

}

double Segment::phi(double s) const
{
    return p.phi + c[0]*s + (c[1]/2.0)*s*s + (c[2]/3.0)*s*s*s;
}

double Segment::x(double s) const
{
    const double epsilon = 1e-8;    // Magic number
    return p.x + AdaptiveSimpson::integrate<double>([&](double t){ return cos(phi(t)); }, 0, s, epsilon);
}

double Segment::y(double s) const
{
    const double epsilon = 1e-8;    // Magic number
    return p.y + AdaptiveSimpson::integrate<double>([&](double t){ return sin(phi(t)); }, 0, s, epsilon);
}

double Segment::l() const
{
    return c[3];
}

double Segment::I() const
{
    return c[3]*(c[0]*c[0] + c[3]*(c[0]*c[1] + c[3]*(2.0/3.0*c[0]*c[2] + 1.0/3.0*c[1]*c[1] + c[3]*(1.0/2.0*c[1]*c[2] + 1.0/5.0*c[2]*c[2]*c[3]))));
}

std::vector<double> Segment::get_c_start(const Point& p0, const Point& p1)
{
    using std::isnan;

    // Given: {s, phi, x, y}
    if(!isnan(p1.s) && !isnan(p1.phi) && !isnan(p1.x) && !isnan(p1.y))
        return get_c_start_x_y(p0, p1.x, p1.y);

    // Given: {s, phi, x}
    if(!isnan(p1.s) && !isnan(p1.phi) && !isnan(p1.x))
        return get_c_start_phi_x(p0, p1.phi, p1.x);

    // Given: {s, phi, y}
    if(!isnan(p1.s) && !isnan(p1.phi) && !isnan(p1.y))
        return get_c_start_phi_y(p0, p1.phi, p1.y);

    // Given: {s, x, y}
    if(!isnan(p1.s) && !isnan(p1.x) && !isnan(p1.y))
        return get_c_start_x_y(p0, p1.x, p1.y);

    // Given: {phi, x, y}
    if (!isnan(p1.phi) && !isnan(p1.x) && !isnan(p1.y))
        return get_c_start_x_y(p0, p1.x, p1.y);

    // Given: {s, phi}
    if(!isnan(p1.s) && !isnan(p1.phi))
        return get_c_start_s_phi(p0, p1.s, p1.phi);

    // Given: {s, x}
    if(!isnan(p1.s) && !isnan(p1.x))
        return get_c_start_s_x(p0, p1.s, p1.x);

    // Given: {s, y}
    if(!isnan(p1.s) && !isnan(p1.y))
        return get_c_start_s_y(p0, p1.s, p1.y);

    // Given: {phi, x}
    if(!isnan(p1.phi) && !isnan(p1.x))
        return get_c_start_phi_x(p0, p1.phi, p1.x);

    // Given: {phi, y}
    if(!isnan(p1.phi) && !isnan(p1.y))
        return get_c_start_phi_y(p0, p1.phi, p1.y);

    // Given: {x, y}
    if(!isnan(p1.x) && !isnan(p1.y))
        return get_c_start_x_y(p0, p1.x, p1.y);

    throw std::invalid_argument("Error in point. At least two entries have to be specified.");
}

std::vector<double> Segment::get_c_start_x_y(const Point& p0, double x1, double y1)
{
    std::cout << "x0 = " << p0.x << ", y0 = " << p0.y << "\n";

    double l = hypot(x1 - p0.x, y1 - p0.y);
    double a = 2.0*(atan2(y1 - p0.y, x1 - p0.x) - p0.phi);
    double c0 = 2.0/l*sin(a/2.0);
    return {c0, 0.0, 0.0, a/c0};
}

std::vector<double> Segment::get_c_start_s_phi(const Point& p0, double s1, double phi1)
{
    double c3 = s1 - p0.s;
    double c0 = (phi1 - p0.phi)/c3;
    return {c0, 0.0, 0.0, c3};
}

std::vector<double> Segment::get_c_start_s_x(const Point& p0, double s1, double x1)
{
    if(x1 > p0.x)
    {
        return get_c_start_phi_x(p0, 0, x1);
    }
    else if(x1 < p0.x)
    {
        return get_c_start_phi_x(p0, M_PI, x1);
    }
    else
    {
        double c3 = s1 - p0.s;
        double c0 = (p0.phi >= 0.0) ? (M_PI - 2*p0.phi)/c3 : (-M_PI - 2*p0.phi)/c3;
        return {c0, 0.0, 0.0, c3};
    }
}

std::vector<double> Segment::get_c_start_s_y(const Point& p0, double s1, double y1)
{
    if(y1 > p0.y)
    {
        return get_c_start_phi_y(p0, M_PI_2, y1);
    }
    else if(y1 < p0.y)
    {
        return get_c_start_phi_y(p0, -M_PI_2, y1);
    }
    else
    {
        double c3 = s1 - p0.s;
        double c0 = (p0.phi > -M_PI_2 && p0.phi < M_PI_2) ? -2.0*p0.phi/c3 : (2.0*M_PI - 2*p0.phi)/c3;
        return {c0, 0.0, 0.0, c3};
    }
}

std::vector<double> Segment::get_c_start_phi_x(const Point& p0, double phi1, double x1)
{
    if(x1 == p0.x)
        throw std::invalid_argument("Segment not uniquely determined. X values must be different.");

    if(phi1 == p0.phi)
        return get_c_start_phi_x(p0, phi1 + 0.1, x1);

    double c0 = (sin(phi1) - sin(p0.phi))/(x1 - p0.x);
    double c3 = (phi1 - p0.phi)/c0;
    if(c3 < 0.0) {
        c3 = 2*M_PI/std::abs(c0) - std::abs(c3);
    }

    return {c0, 0.0, 0.0, c3};
}

std::vector<double> Segment::get_c_start_phi_y(const Point& p0, double phi1, double y1)
{
    if(y1 == p0.y)
        throw std::invalid_argument("Segment not uniquely determined. Y values must be different.");

    if(phi1 == p0.phi)
        return get_c_start_phi_y(p0, phi1 + 0.1, y1);

    double c0 = -(cos(phi1) - cos(p0.phi))/(y1 - p0.y);
    double c3 = (phi1 - p0.phi)/c0;
    if(c3 < 0.0) {
        c3 = 2*M_PI/std::abs(c0) - std::abs(c3);
    }

    return {c0, 0.0, 0.0, c3};
}

ProfileCurve2::ProfileCurve2(std::vector<Point> input)
    : points(input)
{
    using std::isnan;

    // First point: Set unspecified data to zero
    if(isnan(points[0].s))
        points[0].s = 0.0;
    if(isnan(points[0].phi))
        points[0].phi = 0.0;
    if(isnan(points[0].x))
        points[0].x = 0.0;
    if(isnan(points[0].y))
        points[0].y = 0.0;

    for(size_t i = 0; i < points.size()-1; ++i)
    {
        // Start and end point of the segment
        Point& p0 = points[i];
        Point& p1 = points[i+1];

        std::cout << "Start of Segment {" << i << ", " << i+1 << "}: x = " << p0.x << ", y = " << p0.y << "\n";

        // Objective function for the segment
        auto objective = [](const std::vector<double>& c, std::vector<double>& grad, void* data) {
            auto pair = static_cast<std::pair<Point, Point>*>(data);
            return Segment(pair->first, c).I();
        };

        // Constraint functions for the segment
        auto constraint_s = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
            auto pair = static_cast<std::pair<Point, Point>*>(data);
            Segment segment(pair->first, c);
            return segment.l() - (pair->second.s - pair->first.s);
        };

        auto constraint_phi = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
            auto pair = static_cast<std::pair<Point, Point>*>(data);
            Segment segment(pair->first, c);
            return segment.phi(segment.l()) - pair->second.phi;
        };

        auto constraint_x = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
            auto pair = static_cast<std::pair<Point, Point>*>(data);
            Segment segment(pair->first, c);
            return segment.x(segment.l()) - pair->second.x;
        };

        auto constraint_y = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
            auto pair = static_cast<std::pair<Point, Point>*>(data);
            Segment segment(pair->first, c);
            return segment.y(segment.l()) - pair->second.y;
        };

        double f_min;
        std::vector<double> c_min = Segment::get_c_start(p0, p1);
        std::pair<Point, Point> data = std::make_pair(p0, p1);

        nlopt::opt opt(nlopt::algorithm::LN_COBYLA, c_min.size());
        opt.set_min_objective(objective, &data);
        opt.set_xtol_rel(1e-4);    // Magic number
        opt.set_maxeval(100);

        if(!isnan(p1.s))
            opt.add_equality_constraint(constraint_s, &data, 1e-8);  // Magic number
        if(!isnan(p1.phi))
            opt.add_equality_constraint(constraint_phi, &data, 1e-8);  // Magic number
        if(!isnan(p1.x))
            opt.add_equality_constraint(constraint_x, &data, 1e-8);  // Magic number
        if(!isnan(p1.y))
            opt.add_equality_constraint(constraint_y, &data, 1e-8);  // Magic number

        if(opt.optimize(c_min, f_min) < 0) {
            throw std::runtime_error("Failed to determine profile curve.");
        }

        Segment segment(p0, c_min);
        if(segment.l() <= 0) {
            throw std::runtime_error("Failed to determine profile curve.");
        }

        // Set unspecified values of the end point as determined by the resulting segment curve
        if(isnan(p1.s))
            p1.s = p0.s + segment.l();
        if(isnan(p1.phi))
            p1.phi = segment.phi(segment.l());
        if(isnan(p1.x))
            p1.x = segment.x(segment.l());
        if(isnan(p1.y))
            p1.y = segment.y(segment.l());

        segments.push_back(segment);

        std::cout << "End of Segment {" << i << ", " << i+1 << "}: x = " << p1.x << ", y = " << p1.y << "\n";

    }

    for(auto& point: points)
        std::cout << "Point s = " << point.s << "\n";
}

// Arc length at start and end of the curve
double ProfileCurve2::s_min() const
{
    return points.front().s;
}

double ProfileCurve2::s_max() const
{
    return points.back().s;
}

// Evaluate curve at a specific arc length. Returns {x, y, phi}.
Vector<3> ProfileCurve2::operator()(double s) const
{
    index = find_interval(points, [](const Point& point){ return point.s; }, s, index);
    double ds = s - points[index].s;

    return {
        segments[index].x(ds),
        segments[index].y(ds),
        segments[index].phi(ds)
    };
}
