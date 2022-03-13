#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include "config.hpp"
#include <vector>

struct Settings {
    int n_limb_elements = 20;
    int n_string_elements = 25;
    int n_draw_steps = 150;
    double arrow_clamp_force = 0.5;
    double time_span_factor = 1.5;
    double time_step_factor = 0.2;
    double sampling_rate = 1e4;
};

struct Material {
    std::string name = "New material";
    std::string color = "#ff9966";
    double rho = 675.0;
    double E = 12e9;
};

struct Dimensions {
    double brace_height = 0.2;
    double draw_length = 0.7;
    double handle_length = 0.0;
    double handle_setback = 0.0;
    double handle_angle = 0.0;
};

struct Layer {
    std::string name = "New layer";
    int material = 0;
    std::vector<Vector<2>> height{{0.0, 0.015}, {0.5, 0.0125}, {1.0, 0.01}};
};

struct Masses {
    double arrow = 0.025;
    double string_center = 0.0;
    double string_tip = 0.0;
    double limb_tip = 0.0;
};

struct Damping {
    double damping_ratio_limbs = 0.0;
    double damping_ratio_string = 0.0;
};

struct String {
    double strand_stiffness = 3500.0;
    double strand_density = 0.0005;
    int n_strands = 12;
};

struct InputData {
    std::string version = Config::APPLICATION_VERSION;
    std::string comment;
    Settings settings;

    std::vector<Material> materials {
        Material {
            .name = "Material A",
            .rho = 675.0,
            .E = 12e9,
        }
    };

    Dimensions dimensions;

    ProfileInput profile {
        SplineInput {{ 0.0, 0.0 }, { 0.267, 0.01 }, { 0.533, 0.04 }, { 0.8, 0.1 }}
    };

    std::vector<Vector<2>> width{{0.0, 0.06}, {0.5, 0.03}, {1.0, 0.01}};
    std::vector<Layer> layers = {Layer()};
    String string;
    Masses masses;
    Damping damping;

    InputData() = default;
    InputData(const std::string& path);

    void save(const std::string& path) const;
    std::string validate() const;
};
