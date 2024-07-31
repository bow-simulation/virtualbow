#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include <vector>

struct Point {
    double s = 0.0;
    double angle = 0.0;
    Vector<2> position = {0.0, 0.0};
};

class ProfileSegment {
public:
    virtual ~ProfileSegment() = default;

    virtual double s_start() const = 0;
    virtual double s_end() const = 0;

    virtual double curvature(double s) const = 0;
    virtual double angle(double s) const = 0;
    virtual Vector<2> position(double s) const = 0;

    virtual Point point(double s) const;
    virtual std::vector<Point> nodes() const;
};
