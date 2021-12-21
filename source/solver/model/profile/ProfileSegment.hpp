#pragma once
#include "solver/numerics/EigenTypes.hpp"

struct Point {
    double s = 0.0;
    double angle = 0.0;
    Vector<2> position = {0.0, 0.0};
};

class ProfileSegment {
public:
    virtual double s_start() const = 0;
    virtual double s_end() const = 0;

    virtual double angle(double s) const = 0;
    virtual Vector<2> position(double s) const = 0;
};
