#pragma once
#include "solver/numerics/Eigen.hpp"
#include "Segment.hpp"
#include "ClothoidSegment.hpp"
#include "SplineSegment.hpp"

#include <variant>
#include <vector>

// TODO
// * Better index finding in ProfileCurve
// * Maybe get rid of the point class
// * Preserve order of constraints in JSON (Maybe read/write as array)
// * Replace std::array<double, 2> with Vector<2>

using SegmentInput = std::variant<LineInput, ArcInput, ClothoidInput>;
using ProfileInput = std::vector<SegmentInput>;

class ProfileCurve {
public:
    ProfileCurve(const Point& start);
    ProfileCurve(const Point& start, const std::vector<SegmentInput>& inputs);

    void add_segment(const SegmentInput& input);

    double length();
    double angle(double s) const;
    Vector<2> position(double s) const;

private:
    std::vector<Point> nodes;
    std::vector<std::unique_ptr<Segment>> segments;

    std::unique_ptr<Segment> create_segment(const Point& start, const SegmentInput& input);
    size_t find_segment(double s) const;
};
