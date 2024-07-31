#include "ProfileInput.hpp"

void to_json(nlohmann::json& obj, const SegmentInput& input) {
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
        obj["parameters"]["points"] = *value;
    }
    else {
        throw std::runtime_error("Unknown segment type");
    }
}

void from_json(const nlohmann::json& obj, SegmentInput& input) {
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
        input = obj.at("parameters").at("points").get<SplineInput>();
    }
    else {
        throw std::runtime_error("Unknown segment type");
    }
}
