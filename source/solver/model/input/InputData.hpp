#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include "config.hpp"
#include "Settings.hpp"
#include "Layers.hpp"
#include "String.hpp"
#include "Masses.hpp"
#include "Damping.hpp"
#include "Dimensions.hpp"
#include <nlohmann/json.hpp>
#include <vector>

struct InputData {
    std::string version = Config::APPLICATION_VERSION;
    std::string comment;
    Settings settings;
    Dimensions dimensions;
    ProfileInput profile {
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

// Todo: Use "= default" in case it should make it into C++20
static bool operator==(const InputData& lhs, const InputData& rhs) {
    return lhs.version == rhs.version
        && lhs.comment == rhs.comment
        && lhs.settings == rhs.settings
        && lhs.dimensions == rhs.dimensions
        && lhs.profile == rhs.profile
        && lhs.width == rhs.width
        && lhs.layers == rhs.layers
        && lhs.string == rhs.string
        && lhs.damping == rhs.damping
        && lhs.masses == rhs.masses;
}

static bool operator!=(const InputData& lhs, const InputData& rhs) {
    return !operator==(lhs, rhs);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputData, version, comment, settings, dimensions, profile, width, layers, string, masses, damping)
