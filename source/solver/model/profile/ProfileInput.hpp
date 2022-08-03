#pragma once
#include "segments/ClothoidSegment.hpp"
#include "segments/SplineSegment.hpp"

#include <variant>
#include <vector>

using SegmentInput = std::variant<LineInput, ArcInput, SpiralInput, SplineInput>;
using ProfileInput = std::vector<SegmentInput>;

void to_json(nlohmann::json& obj, const SegmentInput& input);
void from_json(const nlohmann::json& obj, SegmentInput& input);
