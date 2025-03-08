#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include <vector>

struct Settings {
    unsigned n_limb_elements = 30;
    unsigned n_limb_eval_points = 100;
    unsigned min_draw_resolution = 100;
    unsigned max_draw_resolution = 100;
    double arrow_clamp_force = 0.5;
    double string_compression_factor = 1e-6;
    double timespan_factor = 1.5;
    double timeout_factor = 10.0;
    double min_timestep = 1e-6;
    double max_timestep = 1e-4;
    unsigned steps_per_period = 250;
};

struct Material {
    std::string name = "New material";
    std::string color = "#ff9966";
    double rho = 675.0;
    double E = 12e9;
};

enum class HandleOrigin {
    Back,
    Belly,
    Profile
};

struct Dimensions {
    double brace_height = 0.2;
    double draw_length = 0.7;
    HandleOrigin handle_origin = HandleOrigin::Belly;
    double handle_length = 0.0;
    double handle_offset = 0.0;
    double handle_angle = 0.0;
};

struct Layer {
    std::string name = "New layer";
    std::string material = "Unnamed";
    std::vector<Vector<2>> height{{0.0, 0.015}, {1.0, 0.01}};
};

struct Masses {
    double arrow = 0.025;
    double string_center = 0.0;
    double string_tip = 0.0;
    double limb_tip = 0.0;
};

struct Damping {
    double damping_ratio_limbs = 0.05;
    double damping_ratio_string = 0.05;
};

struct String {
    double strand_stiffness = 3500.0;
    double strand_density = 0.0005;
    int n_strands = 12;
};

struct InputData {
    std::string comment;
    Settings settings;

    std::vector<Material> materials = { Material() };

    Dimensions dimensions;

    ProfileInput profile = {
        .alignment = SectionBack{},
        .segments = {LineInput {{ LineConstraint::LENGTH, 0.8 }}}
    };

    std::vector<Vector<2>> width = {{0.0, 0.04}, {1.0, 0.01}};

    std::vector<Layer> layers = { Layer() };

    String string;
    Masses masses;
    Damping damping;

    InputData() = default;
    InputData(const std::string& path);

    void save(const std::string& path) const;
};
