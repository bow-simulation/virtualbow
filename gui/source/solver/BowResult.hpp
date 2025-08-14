#pragma once
#include <nlohmann/json.hpp>
#include <optional>

// Serializing std::optional, https://github.com/nlohmann/json/issues/1749
template<typename T>
struct nlohmann::adl_serializer<std::optional<T>> {
    static void from_json(const json& j, std::optional<T>& opt) {
        if(j.is_null()) {
            opt = std::nullopt;
        }
        else {
            opt = j.get<T>();
        }
    }

    static void to_json(json& json, std::optional<T> t) {
        if(t) {
            json = *t;
        }
        else {
            json = nullptr;
        }
    }
};

struct LayerInfo {
    std::string name;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    LayerInfo,
    name
)

struct LimbInfo {
    std::vector<double> length;
    std::vector<std::array<double, 3>> position_eval;
    std::vector<std::array<double, 3>> position_control;
    std::vector<double> width;
    std::vector<double> height;
    std::vector<std::vector<double>> bounds;

    std::vector<double> ratio;
    std::vector<std::vector<double>> heights;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    LimbInfo,
    length,
    position_eval,
    position_control,
    width,
    height,
    bounds,
    ratio,
    heights
)

struct Common {
    LimbInfo limb;
    std::vector<LayerInfo> layers;

    double string_length;
    double string_stiffness;
    double string_mass;
    double limb_mass;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Common,
    limb,
    layers,
    string_length,
    string_stiffness,
    string_mass,
    limb_mass
)

struct States {
    std::vector<double> time;
    std::vector<double> draw_length;

    std::vector<std::vector<std::array<double, 3>>> limb_pos;
    std::vector<std::vector<std::array<double, 3>>> limb_vel;

    std::vector<std::vector<std::array<double, 2>>> string_pos;
    std::vector<std::vector<std::array<double, 2>>> string_vel;

    std::vector<std::vector<std::array<double, 3>>> limb_strain;
    std::vector<std::vector<std::array<double, 3>>> limb_force;

    std::vector<std::vector<std::vector<std::array<double, 2>>>> layer_strain;
    std::vector<std::vector<std::vector<std::array<double, 2>>>> layer_stress;

    std::vector<double> arrow_pos;
    std::vector<double> arrow_vel;
    std::vector<double> arrow_acc;

    std::vector<double> elastic_energy_limbs;
    std::vector<double> elastic_energy_string;

    std::vector<double> kinetic_energy_limbs;
    std::vector<double> kinetic_energy_string;
    std::vector<double> kinetic_energy_arrow;

    std::vector<double> damping_energy_limbs;
    std::vector<double> damping_energy_string;
    std::vector<double> damping_power_limbs;
    std::vector<double> damping_power_string;

    std::vector<double> draw_force;
    std::vector<double> draw_stiffness;
    std::vector<double> grip_force;
    std::vector<double> string_length;
    std::vector<double> string_tip_angle;
    std::vector<double> string_center_angle;
    std::vector<double> string_force;
    std::vector<double> strand_force;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    States,
    time,
    draw_length,
    limb_pos,
    limb_vel,
    string_pos,
    string_vel,
    limb_strain,
    limb_force,
    layer_strain,
    layer_stress,
    arrow_pos,
    arrow_vel,
    arrow_acc,
    elastic_energy_limbs,
    elastic_energy_string,
    kinetic_energy_limbs,
    kinetic_energy_string,
    kinetic_energy_arrow,
    damping_energy_limbs,
    damping_energy_string,
    damping_power_limbs,
    damping_power_string,
    draw_force,
    draw_stiffness,
    grip_force,
    string_length,
    string_tip_angle,
    string_center_angle,
    string_force,
    strand_force
)

struct Statics {
    States states;

    double final_draw_force;
    double final_drawing_work;
    double storage_factor;

    std::tuple<double, unsigned> max_string_force;
    std::tuple<double, unsigned> max_strand_force;
    std::tuple<double, unsigned> max_draw_force;
    std::tuple<double, unsigned> min_grip_force;
    std::tuple<double, unsigned> max_grip_force;

    std::vector<std::tuple<double, std::array<unsigned, 3>>> min_layer_stresses;
    std::vector<std::tuple<double, std::array<unsigned, 3>>> max_layer_stresses;
    std::vector<std::tuple<double, std::array<unsigned, 3>>> min_layer_strains;
    std::vector<std::tuple<double, std::array<unsigned, 3>>> max_layer_strains;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Statics,
    states,
    final_draw_force,
    final_drawing_work,
    storage_factor,
    max_string_force,
    max_strand_force,
    max_draw_force,
    min_grip_force,
    max_grip_force,
    min_layer_stresses,
    max_layer_stresses,
    min_layer_strains,
    max_layer_strains
)

struct ArrowDeparture {
    size_t state_idx;
    double arrow_pos;
    double arrow_vel;
    double kinetic_energy_arrow;
    double elastic_energy_limbs;
    double kinetic_energy_limbs;
    double damping_energy_limbs;
    double elastic_energy_string;
    double kinetic_energy_string;
    double damping_energy_string;
    double energy_efficiency;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ArrowDeparture,
    state_idx,
    arrow_pos,
    arrow_vel,
    kinetic_energy_arrow,
    elastic_energy_limbs,
    kinetic_energy_limbs,
    damping_energy_limbs,
    elastic_energy_string,
    kinetic_energy_string,
    damping_energy_string,
    energy_efficiency
)

struct Dynamics {
    States states;

    std::optional<ArrowDeparture> arrow_departure;

    std::tuple<double, unsigned> max_string_force;
    std::tuple<double, unsigned> max_strand_force;
    std::tuple<double, unsigned> max_draw_force;
    std::tuple<double, unsigned> min_grip_force;
    std::tuple<double, unsigned> max_grip_force;

    std::vector<std::tuple<double, std::array<unsigned, 3>>> min_layer_stresses;
    std::vector<std::tuple<double, std::array<unsigned, 3>>> max_layer_stresses;
    std::vector<std::tuple<double, std::array<unsigned, 3>>> min_layer_strains;
    std::vector<std::tuple<double, std::array<unsigned, 3>>> max_layer_strains;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Dynamics,
    states,
    arrow_departure,
    max_string_force,
    max_strand_force,
    max_draw_force,
    min_grip_force,
    max_grip_force,
    min_layer_stresses,
    max_layer_stresses,
    min_layer_strains,
    max_layer_strains
)

struct BowResult {
    Common common;
    std::optional<Statics> statics;
    std::optional<Dynamics> dynamics;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    BowResult,
    common,
    statics,
    dynamics
)
