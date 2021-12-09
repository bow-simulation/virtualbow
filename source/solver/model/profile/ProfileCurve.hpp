#pragma once
#include "solver/numerics/Eigen.hpp"
#include "ProfileSegment.hpp"
#include "ProfileInput.hpp"

#include <vector>
#include <memory>

// TODO
// * Better index finding in ProfileCurve
// * Maybe get rid of the point class
// * Preserve order of constraints in JSON (Maybe read/write as array)
// * Replace std::array<double, 2> with Vector<2>
// * (De)serialize inputs, add to InputData
// * Rename LineInInput -> LineSegmentInput, etc.

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
    std::vector<std::unique_ptr<ProfileSegment>> segments;

    std::unique_ptr<ProfileSegment> create_segment(const Point& start, const SegmentInput& input);
    size_t find_segment(double s) const;
};
