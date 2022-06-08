#include "ProfileSegment.hpp"

Point ProfileSegment::point(double s) const {
    return {
        .s = s,
        .angle = angle(s),
        .position = position(s)
    };
}

std::vector<Point> ProfileSegment::nodes() const {
    return {
        point(s_start()),
        point(s_end())
    };
}
