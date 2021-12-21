#pragma once
#include "ClothoidSegment.hpp"
#include "SplineSegment.hpp"

#include <variant>
#include <vector>

using SegmentInput = std::variant<LineInput, ArcInput, SpiralInput, SplineInput>;
using ProfileInput = std::vector<SegmentInput>;

inline void to_json(nlohmann::json& obj, const SegmentInput& input) {
    if(auto value = std::get_if<LineInput>(&input)) {
        obj["type"] = "line";
        obj["parameters"] = *value;
    }
    else if(auto value = std::get_if<ArcInput>(&input)) {
        obj["type"] = "arc";
        obj["parameters"] = *value;
    }
    else if(auto value = std::get_if<SpiralInput>(&input)) {
        obj["type"] = "spiral";
        obj["parameters"] = *value;
    }
    else if(auto value = std::get_if<SplineInput>(&input)) {
        obj["type"] = "spline";
        obj["parameters"] = *value;
    }
    else {
        throw std::runtime_error("Unknown segment type");
    }
}

inline void from_json(const nlohmann::json& obj, SegmentInput& input) {
    if(obj.at("type") == "line") {
        input = obj.at("parameters").get<LineInput>();
    }
    else if(obj.at("type") == "arc") {
        input = obj.at("parameters").get<ArcInput>();
    }
    else if(obj.at("type") == "spiral") {
        input = obj.at("parameters").get<SpiralInput>();
    }
    else if(obj.at("type") == "spline") {
        input = obj.at("parameters").get<SplineInput>();
    }
    else {
        throw std::runtime_error("Unknown segment type");
    }
}
