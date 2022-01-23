#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include "config.hpp"
#include <vector>

struct Settings {
    unsigned n_limb_elements = 20;
    unsigned n_string_elements = 25;
    unsigned n_draw_steps = 150;
    double arrow_clamp_force = 0.5;
    double time_span_factor = 1.5;
    double time_step_factor = 0.2;
    double sampling_rate = 1e4;
};

struct Material {
    std::string name = "New material";
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
    std::vector<Vector<2>> height{{0.0, 0.015}, {1.0, 0.01}};
    double rho = 675.0;
    double E = 12e9;
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
        },
        Material {
            .name = "Material B",
            .rho = 675.0,
            .E = 12e9,
        },
        Material {
            .name = "Material C",
            .rho = 675.0,
            .E = 12e9,
        }
    };

    Dimensions dimensions;

    ProfileInput profile {
        SplineInput {
            { 0.0, 0.0 },
            { 1.0, 0.1 },
            { 2.0, 0.4 },
            { 3.0, 0.9 }
        },
        SplineInput {
            { 0.0, 0.0 },
            { 1.0, 0.1 },
            { 2.0, 0.4 },
            { 3.0, 0.9 }
        }
    };

    std::vector<Vector<2>> width{{0.0, 0.06}, {1.0, 0.01}};
    std::vector<Layer> layers = {Layer()};
    String string;
    Masses masses;
    Damping damping;

    InputData() = default;
    InputData(const std::string& path);

    void save(const std::string& path) const;
    std::string validate() const;
};
