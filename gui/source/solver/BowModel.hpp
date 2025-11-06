#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <array>
#include <list>
#include <variant>

using Points = std::vector<std::array<double, 2>>;

using Width = Points;

using Height = Points;

struct Settings {
    int num_limb_elements;
    int num_limb_eval_points;
    int min_draw_resolution;
    int max_draw_resolution;
    double arrow_clamp_force;
    double string_compression_factor;
    double timespan_factor;
    double timeout_factor;
    double min_timestep;
    double max_timestep;
    int steps_per_period;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Settings,
    num_limb_elements,
    num_limb_eval_points,
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

enum class HandleReference {
    Back,
    Belly,
    Profile
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    HandleReference, {
        {HandleReference::Back, "back"},
        {HandleReference::Belly, "belly"},
        {HandleReference::Profile, "profile"}
    }
)

struct Dimensions {
    HandleReference handle_reference;
    double handle_length;
    double handle_offset;
    double handle_angle;
    double brace_height;
    double draw_length;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Dimensions,
    handle_reference,
    handle_length,
    handle_offset,
    handle_angle,
    brace_height,
    draw_length
)

struct Mass { double value; };
struct MassPerForce { double value; };
struct MassPerEnergy { double value; };
using ArrowMass = std::variant<Mass, MassPerForce, MassPerEnergy>;

void to_json(nlohmann::json& obj, const ArrowMass& input);
void from_json(const nlohmann::json& obj, ArrowMass& output);

struct Masses {
    ArrowMass arrow;
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
void from_json(const nlohmann::json& obj, ProfileSegment& output);

struct Profile {
    std::list<ProfileSegment> segments;    // List to keep references valid when swapping around elements
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Profile,
    segments
)

struct SectionBack {};
struct SectionBelly {};
struct SectionCenter {};
struct LayerBack { std::string layer; };
struct LayerBelly { std::string layer; };
struct LayerCenter { std::string layer; };
using LayerAlignment = std::variant<SectionBack, SectionBelly, SectionCenter, LayerBack, LayerBelly, LayerCenter>;

void to_json(nlohmann::json& obj, const LayerAlignment& input);
void from_json(const nlohmann::json& obj, LayerAlignment& output);

struct Material {
    std::string name;
    std::string color;
    double density;
    double youngs_modulus;
    double shear_modulus;
    double tensile_strength;
    double compressive_strength;
    double safety_margin;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Material,
    name,
    color,
    density,
    youngs_modulus,
    shear_modulus,
    tensile_strength,
    compressive_strength,
    safety_margin
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

struct Section {
    LayerAlignment alignment;
    Width width;
    std::list<Material> materials;    // List to keep references valid when swapping around elements
    std::list<Layer> layers;    // List to keep references valid when swapping around elements
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Section,
    alignment,
    width,
    materials,
    layers
)

struct BowModel {
    std::string comment;
    Settings settings;
    Dimensions dimensions;
    Profile profile;
    Section section;
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
    dimensions,
    profile,
    section,
    string,
    masses,
    damping
)
