#pragma once
#include "solver/numerics/EigenTypes.hpp"
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
    ProfileCurve();
    ProfileCurve(const std::vector<SegmentInput>& inputs);

    void add_segment(const SegmentInput& input);
    const std::vector<std::unique_ptr<ProfileSegment>>& get_segments() const;
    const std::vector<Point>& get_nodes() const;

    double length() const;
    double curvature(double s) const;
    double angle(double s) const;
    Vector<2> position(double s) const;

private:
    std::vector<Point> nodes;
    std::vector<std::unique_ptr<ProfileSegment>> segments;

    std::unique_ptr<ProfileSegment> create_segment(const Point& start, const SegmentInput& input);

    mutable size_t index = 0;
    size_t find_segment(double s) const;
};
