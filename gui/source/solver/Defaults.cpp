#include "Defaults.hpp"

ProfileSegment createDefaultSegment(SegmentType type) {
    switch(type) {
    case SegmentType::Line:
        return Line{
            .length = 0.1
        };

    case SegmentType::Arc:
        return Arc {
            .length = 0.1,
            .radius = 0.1
        };

    case SegmentType::Spiral:
        return Spiral {
            .length = 0.1,
            .radius_start = 0.1,
            .radius_end = 0.1
        };

    case SegmentType::Spline:
        return Spline {
            .points = {{0.0, 0.0}, {1.0, 0.0}}
        };
    }

    throw std::invalid_argument("Unknown enum variant");
}
