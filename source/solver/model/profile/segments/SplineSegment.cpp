#include "SplineSegment.hpp"
#include "solver/numerics/Linspace.hpp"

SplineSegment::SplineSegment(const Point& start, const SplineInput& input) {

    // TODO: Add point 0 if missing

    std::vector<double> t = linspace(0.0, 1.0, input.size());
    std::vector<double> x; x.reserve(input.size());
    std::vector<double> y; y.reserve(input.size());

    for(auto& point: input) {
        x.push_back(start.position[0] + point[0]);
        y.push_back(start.position[0] + point[1]);
    }

    // TODO: Set boundary conditions

    spline_x = tk::spline(t, x);
    spline_y = tk::spline(t, y);

    // Calculate arc length s over curve parameter t

    const int k = 50*(t.size() - 1);    // Magic number
    std::vector<double> s(k, start.s);
    t = linspace(0.0, 1.0, k);

    auto dsdt = [&](double t) {
        return std::hypot(spline_x.deriv(1, t), spline_y.deriv(1, t));
    };

    for(size_t i = 1; i < k; ++i) {
        s[i] = s[i-1] + (t[i] - t[i-1])/6.0*(dsdt(t[i-1]) + 4.0*dsdt((t[i] + t[i-1])/2.0) + dsdt(t[i]));    // Simpson method
    }

    spline_t = tk::spline(s, t);
}

double SplineSegment::s_start() const {
    return spline_t.get_x_min();
}

double SplineSegment::s_end() const {
    return spline_t.get_x_max();
}

double SplineSegment::curvature(double s) const {
    return 0.0;
}

double SplineSegment::angle(double s) const {
    return 0.0;
}

Vector<2> SplineSegment::position(double s) const {
    double t = spline_t(s);
    return { spline_x(t), spline_y(t) };
}
