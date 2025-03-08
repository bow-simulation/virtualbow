#pragma once
#include "segments/ClothoidSegment.hpp"
#include "segments/SplineSegment.hpp"

#include <variant>
#include <vector>

struct SectionBack{};
struct SectionBelly{};
struct SectionCenter{};
struct LayerBack{ std::string layer; };
struct LayerBelly{ std::string layer; };
struct LayerCenter{ std::string layer; };

using SegmentInput = std::variant<LineInput, ArcInput, SpiralInput, SplineInput>;
using ProfileAlignment = std::variant<SectionBack, SectionBelly, SectionCenter, LayerBack, LayerBelly, LayerCenter>;

struct ProfileInput {
    ProfileAlignment alignment;
    std::vector<SegmentInput> segments;
};

void to_json(nlohmann::json& obj, const SegmentInput& input);
void from_json(const nlohmann::json& obj, SegmentInput& input);

void to_json(nlohmann::json& obj, const ProfileAlignment& input);
void from_json(const nlohmann::json& obj, ProfileAlignment& input);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProfileInput, alignment, segments)
