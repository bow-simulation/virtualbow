#pragma once
#include "solver/model/profile/ProfileSegment.hpp"
#include "solver/numerics/EigenSerialize.hpp"
#include "solver/numerics/CubicSpline.hpp"
#include <vector>

using SplineInput = std::vector<Vector<2>>;

class SplineSegment: public ProfileSegment {
public:
    SplineSegment(const Point& start, const SplineInput& input);

    double s_start() const override;
    double s_end() const override;

    double curvature(double s) const override;
    double angle(double s) const override;
    Vector<2> position(double s) const override;

    std::vector<Point> nodes() const override;

private:
    CubicSpline spline_t;    // t(s)
    CubicSpline spline_x;    // x(t)
    CubicSpline spline_y;    // y(t)
};
