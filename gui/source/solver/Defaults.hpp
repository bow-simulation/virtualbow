#pragma once
#include "BowModel.hpp"

enum class SegmentType {
    Line,
    Arc,
    Spiral,
    Spline
};

ProfileSegment createDefaultSegment(SegmentType type);

// TODO: Move that logic here from the solver
// BowModel createDefaultBow();
