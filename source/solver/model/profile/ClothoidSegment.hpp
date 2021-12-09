#pragma once
#include "Segment.hpp"
#include <unordered_map>

enum class ClothoidConstraint{ LENGTH, K_START, K_END };
using ClothoidInput = std::unordered_map<ClothoidConstraint, double>;

enum class ArcConstraint{ LENGTH, K_START };
using ArcInput = std::unordered_map<ArcConstraint, double>;

enum class LineConstraint{ LENGTH };
using LineInput = std::unordered_map<LineConstraint, double>;

// Segment with curvature linearly varying over arc length.
// Represents a clothoid, circular arc or a straight line depending on the choice of curvature.
class ClothoidSegment: public Segment {
public:
    ClothoidSegment(const Point& start, const ClothoidInput& input);
    ClothoidSegment(const Point& start, const ArcInput& input);
    ClothoidSegment(const Point& start, const LineInput& input);
    ClothoidSegment(const Point& start, double l, double k0, double k1);

    double s_start() const override;
    double s_end() const override;

    double angle(double s) const override;
    Vector<2> position(double s) const override;

private:
    double x0;
    double y0;

    double s0;
    double s1;

    double a;
    double b;
    double c;
};
