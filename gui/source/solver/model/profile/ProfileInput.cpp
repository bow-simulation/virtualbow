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

void to_json(nlohmann::json& obj, const ProfileAlignment& input) {
    if(auto value = std::get_if<SectionBack>(&input)) {
        obj["type"] = "section_back";
    }
    else if(auto value = std::get_if<SectionBelly>(&input)) {
        obj["type"] = "section_belly";
    }
    else if(auto value = std::get_if<SectionCenter>(&input)) {
        obj["type"] = "section_center";
    }
    else if(auto value = std::get_if<LayerBack>(&input)) {
        obj["type"] = "layer_back";
        obj["layer"] = value->layer;
    }
    else if(auto value = std::get_if<LayerBelly>(&input)) {
        obj["type"] = "layer_belly";
        obj["layer"] = value->layer;
    }
    else if(auto value = std::get_if<LayerCenter>(&input)) {
        obj["type"] = "layer_center";
        obj["layer"] = value->layer;
    }
    else {
        throw std::runtime_error("Unknown alignment type");
    }
}

void from_json(const nlohmann::json& obj, ProfileAlignment& input) {
    if(obj.at("type") == "section_back") {
        input = SectionBack{};
    }
    else if(obj.at("type") == "section_belly") {
        input = SectionBelly{};
    }
    else if(obj.at("type") == "section_center") {
        input = SectionCenter{};
    }
    else if(obj.at("type") == "layer_back") {
        input = LayerBack {
            .layer = obj.at("layer")
        };
    }
    else if(obj.at("type") == "layer_belly") {
        input = LayerBelly {
            .layer = obj.at("layer")
        };
    }
    else if(obj.at("type") == "layer_center") {
        input = LayerCenter {
            .layer = obj.at("layer")
        };
    }
    else {
        throw std::runtime_error("Unknown alignment type");
    }
}
