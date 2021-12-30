#include "ProfileCurve.hpp"

ProfileCurve::ProfileCurve()
    : nodes({Point()})
{

}

ProfileCurve::ProfileCurve(const std::vector<SegmentInput>& inputs)
    : ProfileCurve()
{
    for(auto& input: inputs) {
        add_segment(input);
    }
}

void ProfileCurve::add_segment(const SegmentInput& input) {
    auto segment = create_segment(nodes.back(), input);
    nodes.push_back(Point {
        .s = segment->s_end(),
        .angle = segment->angle(segment->s_end()),
        .position = segment->position(segment->s_end())
    });
    segments.push_back(std::move(segment));
}

double ProfileCurve::length() const {
    return nodes.back().s;
}

double ProfileCurve::curvature(double s) const {
    size_t index = find_segment(s);
    return segments[index]->curvature(s);
}

double ProfileCurve::angle(double s) const {
    size_t index = find_segment(s);
    return segments[index]->angle(s);
}

Vector<2> ProfileCurve::position(double s) const {
    size_t index = find_segment(s);
    return segments[index]->position(s);
}

std::unique_ptr<ProfileSegment> ProfileCurve::create_segment(const Point& start, const SegmentInput& input) {
    if(auto value = std::get_if<LineInput>(&input)) {
        return std::make_unique<ClothoidSegment>(start, *value);
    }
    if(auto value = std::get_if<ArcInput>(&input)) {
        return std::make_unique<ClothoidSegment>(start, *value);
    }
    if(auto value = std::get_if<SpiralInput>(&input)) {
        return std::make_unique<ClothoidSegment>(start, *value);
    }
    if(auto value = std::get_if<SplineInput>(&input)) {
        throw std::runtime_error("Spline segment not yet implemented");
    }

    throw std::runtime_error("Unknown segment type");
}

size_t ProfileCurve::find_segment(double s) const {
    for(size_t i = 0; i < segments.size(); ++i) {
        if(s >= nodes[i].s && s <= nodes[i+1].s) {
            return i;
        }
    }

    throw std::invalid_argument("Segment index not found");
}
