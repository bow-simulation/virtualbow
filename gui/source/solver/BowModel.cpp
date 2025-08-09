#include "BowModel.hpp"

bool BowModel::isValidMaterialName(const std::string& name) const {
    if(name.empty()) {
        return false;
    }

    for(auto material: section.materials) {
        if(material.name == name) {
            return false;
        }
    }

    return true;
}

std::string BowModel::generateMaterialName() const {
    size_t index = 1;
    std::string name = "";

    while(true) {
        name = "Material " + std::to_string(index);
        if(isValidMaterialName(name)) {
            return name;
        }

        index += 1;
    }
}

bool BowModel::isValidLayerName(const std::string& name) const {
    if(name.empty()) {
        return false;
    }

    for(auto layer: section.layers) {
        if(layer.name == name) {
            return false;
        }
    }

    return true;
}

std::string BowModel::generateLayerName() const {
    size_t index = 1;
    std::string name = "";

    while(true) {
        name = "Layer " + std::to_string(index);
        if(isValidLayerName(name)) {
            return name;
        }

        index += 1;
    }
}

void to_json(nlohmann::json& obj, const ProfileSegment& input) {
    if(auto value = std::get_if<Line>(&input)) {
        obj["type"] = "line";
        obj["parameters"] = *value;
    }
    else if(auto value = std::get_if<Arc>(&input)) {
        obj["type"] = "arc";
        obj["parameters"] = *value;
    }
    else if(auto value = std::get_if<Spiral>(&input)) {
        obj["type"] = "spiral";
        obj["parameters"] = *value;
    }
    else if(auto value = std::get_if<Spline>(&input)) {
        obj["type"] = "spline";
        obj["parameters"] = *value;
    }
    else {
        throw std::runtime_error("Unknown segment type");
    }
}

void from_json(const nlohmann::json& obj, ProfileSegment& input) {
    if(obj.at("type") == "line") {
        input = obj.at("parameters").get<Line>();
    }
    else if(obj.at("type") == "arc") {
        input = obj.at("parameters").get<Arc>();
    }
    else if(obj.at("type") == "spiral") {
        input = obj.at("parameters").get<Spiral>();
    }
    else if(obj.at("type") == "spline") {
        input = obj.at("parameters").get<Spline>();
    }
    else {
        throw std::runtime_error("Unknown segment type");
    }
}

void to_json(nlohmann::json& obj, const LayerAlignment& input) {
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

void from_json(const nlohmann::json& obj, LayerAlignment& input) {
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
