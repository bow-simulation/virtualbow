#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <array>
#include <variant>

using Width = std::vector<std::array<double, 2>>;

using Height = std::vector<std::array<double, 2>>;

struct Settings {
    unsigned n_limb_elements;
    unsigned n_limb_eval_points;
    unsigned min_draw_resolution;
    unsigned max_draw_resolution;
    double arrow_clamp_force;
    double string_compression_factor;
    double timespan_factor;
    double timeout_factor;
    double min_timestep;
    double max_timestep;
    unsigned steps_per_period;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Settings,
    n_limb_elements,
    n_limb_eval_points,
    min_draw_resolution,
    max_draw_resolution,
    arrow_clamp_force,
    string_compression_factor,
    timespan_factor,
    timeout_factor,
    min_timestep,
    max_timestep,
    steps_per_period
)

struct Material {
    std::string name;
    std::string color;
    double rho;
    double E;
    double G;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Material,
    name,
    color,
    rho,
    E,
    G
)

enum class HandleOrigin {
    Back,
    Belly,
    Profile
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    HandleOrigin, {
        {HandleOrigin::Back, "back"},
        {HandleOrigin::Belly, "belly"},
        {HandleOrigin::Profile, "profile"}
    }
)

struct Dimensions {
    double brace_height;
    double draw_length;
    HandleOrigin handle_origin;
    double handle_length;
    double handle_offset;
    double handle_angle;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Dimensions,
    brace_height,
    draw_length,
    handle_origin,
    handle_length,
    handle_offset,
    handle_angle
)

struct Layer {
    std::string name;
    std::string material;
    Height height;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Layer,
    name,
    material,
    height
)

struct Masses {
    double arrow;
    double string_center;
    double string_tip;
    double limb_tip;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Masses,
    arrow,
    string_center,
    string_tip,
    limb_tip
)

struct Damping {
    double damping_ratio_limbs;
    double damping_ratio_string;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Damping,
    damping_ratio_limbs,
    damping_ratio_string
)

struct String {
    double strand_stiffness;
    double strand_density;
    int n_strands;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    String,
    strand_stiffness,
    strand_density,
    n_strands
)

struct SectionBack {};
struct SectionBelly {};
struct SectionCenter {};
struct LayerBack { std::string layer; };
struct LayerBelly { std::string layer; };
struct LayerCenter { std::string layer; };
using ProfileAlignment = std::variant<SectionBack, SectionBelly, SectionCenter, LayerBack, LayerBelly, LayerCenter>;

void to_json(nlohmann::json& obj, const ProfileAlignment& input);
void from_json(const nlohmann::json& obj, ProfileAlignment& input);

struct Line {
    double length;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Line,
    length
)

struct Arc {
    double length;
    double radius;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Arc,
    length,
    radius
)

struct Spiral {
    double length;
    double radius_start;
    double radius_end;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Spiral,
    length,
    radius_start,
    radius_end
)

struct Spline {
    std::vector<std::array<double, 2>> points;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Spline,
    points
)

using ProfileSegment = std::variant<Line, Arc, Spiral, Spline>;

void to_json(nlohmann::json& obj, const ProfileSegment& input);
void from_json(const nlohmann::json& obj, ProfileSegment& input);

struct Profile {
    ProfileAlignment alignment;
    std::vector<ProfileSegment> segments;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Profile,
    alignment,
    segments
)

struct BowModel {
    std::string comment;
    Settings settings;
    std::vector<Material> materials;
    Dimensions dimensions;
    Profile profile;
    Width width;
    std::vector<Layer> layers;
    String string;
    Masses masses;
    Damping damping;

    bool isValidMaterialName(const std::string& name) const;
    std::string generateMaterialName() const;

    bool isValidLayerName(const std::string& name) const;
    std::string generateLayerName() const;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    BowModel,
    comment,
    settings,
    materials,
    dimensions,
    profile,
    width,
    layers,
    string,
    masses,
    damping
)
