#include "BowModel.hpp"

BowModel BowModel::example() {
    return BowModel {
        .comment = "",
        .settings = Settings {
             .num_limb_elements = 30,
             .num_limb_eval_points = 250,
             .min_draw_resolution = 100,
             .max_draw_resolution = 100,
             .static_iteration_tolerance = 1e-6,
             .arrow_clamp_force = 0.5,
             .string_compression_factor = 1e-6,
             .timespan_factor = 1.5,
             .timeout_factor = 10.0,
             .min_timestep = 1e-6,
             .max_timestep = 1e-4,
             .steps_per_period = 250,
             .dynamic_iteration_tolerance = 1e-6
        },
        .handle = FlexibleHandle{},
        .draw = Draw {
            .brace_height = 0.2,
            .draw_length = StandardDrawLength {
                .value = 0.7
            }
        },
        .profile = Profile {
            .segments = {
                Line{
                    .length=0.8
                }
            }
        },
        .section = {
            .alignment = SectionBack{},
            .width = {{0.0, 0.04}, {1.0, 0.01}},
            .materials = {
                Material {
                    .name = "Material 1",
                    .color = "#d0b391",
                    .density = 675.0,
                    .youngs_modulus = 12e9,
                    .shear_modulus = 6e9,
                    .tensile_strength = 0.0,
                    .compressive_strength = 0.0,
                    .margin_of_safety = 0.0
                }
            },
            .layers = {
                Layer {
                    .name = "Layer 1",
                    .material = "Material 1",
                    .height = {{0.0, 0.015}, {1.0, 0.01}}
                }
            },
        },
        .string = String {
            .strand_stiffness = 3500.0,
            .strand_density = 0.0005,
            .n_strands = 12
        },
        .masses = Masses {
            .arrow = Mass{ .value = 0.025 },
            .string_center = 0.0,
            .string_tip = 0.0,
            .limb_tip = 0.0
        },
        .damping = Damping {
            .damping_ratio_limbs = 0.05,
            .damping_ratio_string = 0.05
        },
    };
}

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

void to_json(nlohmann::json& obj, const Handle& input) {
    if(auto value = std::get_if<FlexibleHandle>(&input)) {
        obj["type"] = "flexible";
    }
    else if(auto value = std::get_if<RigidHandle>(&input)) {
        obj["type"] = "rigid";
        obj["value"] = *value;
    }
    else {
        throw std::runtime_error("Unknown handle type");
    }
}

void from_json(const nlohmann::json& obj, Handle& output) {
    if(obj.at("type") == "flexible") {
        output = FlexibleHandle{};
    }
    else if(obj.at("type") == "rigid") {
        output = obj.at("value").get<RigidHandle>();
    }
    else {
        throw std::runtime_error("Unknown handle type");
    }
}

void to_json(nlohmann::json& obj, const DrawLength& input) {
    if(auto value = std::get_if<StandardDrawLength>(&input)) {
        obj["type"] = "standard";
        obj["value"] = value->value;
    }
    else if(auto value = std::get_if<AMODrawLength>(&input)) {
        obj["type"] = "amo";
        obj["value"] = value->value;
    }
    else {
        throw std::runtime_error("Unknown draw length type");
    }
}

void from_json(const nlohmann::json& obj, DrawLength& output) {
    if(obj.at("type") == "standard") {
        output = StandardDrawLength { .value = obj.at("value") };
    }
    else if(obj.at("type") == "amo") {
        output = AMODrawLength { .value = obj.at("value") };
    }
    else {
        throw std::runtime_error("Unknown draw length type");
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

void from_json(const nlohmann::json& obj, ProfileSegment& output) {
    if(obj.at("type") == "line") {
        output = obj.at("parameters").get<Line>();
    }
    else if(obj.at("type") == "arc") {
        output = obj.at("parameters").get<Arc>();
    }
    else if(obj.at("type") == "spiral") {
        output = obj.at("parameters").get<Spiral>();
    }
    else if(obj.at("type") == "spline") {
        output = obj.at("parameters").get<Spline>();
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

void from_json(const nlohmann::json& obj, LayerAlignment& output) {
    if(obj.at("type") == "section_back") {
        output = SectionBack{};
    }
    else if(obj.at("type") == "section_belly") {
        output = SectionBelly{};
    }
    else if(obj.at("type") == "section_center") {
        output = SectionCenter{};
    }
    else if(obj.at("type") == "layer_back") {
        output = LayerBack {
            .layer = obj.at("layer")
        };
    }
    else if(obj.at("type") == "layer_belly") {
        output = LayerBelly {
            .layer = obj.at("layer")
        };
    }
    else if(obj.at("type") == "layer_center") {
        output = LayerCenter {
            .layer = obj.at("layer")
        };
    }
    else {
        throw std::runtime_error("Unknown alignment type");
    }
}

void to_json(nlohmann::json& obj, const ArrowMass& input) {
    if(auto value = std::get_if<Mass>(&input)) {
        obj["type"] = "mass";
        obj["value"] = value->value;
    }
    else if(auto value = std::get_if<MassPerForce>(&input)) {
        obj["type"] = "mass_per_force";
        obj["value"] = value->value;
    }
    else if(auto value = std::get_if<MassPerEnergy>(&input)) {
        obj["type"] = "mass_per_energy";
        obj["value"] = value->value;
    }
    else {
        throw std::runtime_error("Unknown alignment type");
    }
}

void from_json(const nlohmann::json& obj, ArrowMass& output) {
    if(obj.at("type") == "mass") {
        output = Mass {
            .value = obj.at("value")
        };
    }
    else if(obj.at("type") == "mass_per_force") {
        output = MassPerForce {
            .value = obj.at("value")
        };
    }
    else if(obj.at("type") == "mass_per_energy") {
        output = MassPerEnergy {
            .value = obj.at("value")
        };
    }
    else {
        throw std::runtime_error("Unknown alignment type");
    }
}
