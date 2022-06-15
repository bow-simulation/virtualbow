#include "SplineSegment.hpp"
#include "solver/numerics/Linspace.hpp"

#include <iostream>

SplineSegment::SplineSegment(const Point& start, const SplineInput& input) {
    std::vector<double> x; x.reserve(input.size() + 1);
    std::vector<double> y; y.reserve(input.size() + 1);

    // Add point (0, 0) if missing
    if(input.size() > 0 && !input[0].isZero()) {
        x.push_back(start.position[0]);
        y.push_back(start.position[1]);
    }

    // Add points from input, relative to starting point
    for(auto& point: input) {
        x.push_back(start.position[0] + point[0]);
        y.push_back(start.position[1] + point[1]);
    }

    if(x.size() < 2) {
        throw std::invalid_argument("At least two points are required");
    }

    double N = std::hypot(x[1] - x[0], y[1] - y[0]);         // Length of normal vector at start (magic number motivated by cubic Bezier curve)
    std::vector<double> t = linspace(0.0, 1.0, x.size());    // Linearly spaced curve parameter
    spline_x = CubicSpline(t, x, false, BoundaryType::FIRST_DERIVATIVE, N*cos(start.angle));
    spline_y = CubicSpline(t, y, false, BoundaryType::FIRST_DERIVATIVE, N*sin(start.angle));

    // Calculate arc length s over curve parameter t

    const size_t k = 50*(t.size() - 1);    // Magic number
    std::vector<double> s(k, start.s);
    t = linspace(0.0, 1.0, k);

    auto dsdt = [&](double t) {
        return std::hypot(spline_x.deriv1(t), spline_y.deriv1(t));
    };

    for(size_t i = 1; i < k; ++i) {
        s[i] = s[i-1] + (t[i] - t[i-1])/6.0*(dsdt(t[i-1]) + 4.0*dsdt((t[i] + t[i-1])/2.0) + dsdt(t[i]));    // Simpson method
    }

    spline_t = CubicSpline(s, t);
}

double SplineSegment::s_start() const {
    return spline_t.arg_min();
}

double SplineSegment::s_end() const {
    return spline_t.arg_max();
}

double SplineSegment::curvature(double s) const {
    double t = spline_t(s);
    double dxdt = spline_x.deriv1(t);
    double dydt = spline_y.deriv1(t);

    double dxdt2 = spline_x.deriv2(t);
    double dydt2 = spline_y.deriv2(t);

    return (dxdt*dydt2 - dxdt2*dydt)/std::pow(dxdt*dxdt + dydt*dydt, 1.5);
}

double SplineSegment::angle(double s) const {
    double t = spline_t(s);
    return std::atan2(spline_y.deriv1(t), spline_x.deriv1(t));
}

Vector<2> SplineSegment::position(double s) const {
    double t = spline_t(s);
    return { spline_x(t), spline_y(t) };
}

std::vector<Point> SplineSegment::nodes() const {
    std::vector<Point> nodes;
    for(double t: spline_x.args()) {
        nodes.push_back({
            .s = 0.0,        // TODO
            .angle = 0.0,    // TODO
            .position = { spline_x(t), spline_y(t) }
        });
    }

    return nodes;
}
