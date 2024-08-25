#pragma once
#include <nlohmann/json.hpp>
#include <optional>

// Serializing std::optional
// TODO: Can be removed after updating to newest version of nlohmann::json
// https://github.com/nlohmann/json/issues/1749
template<typename T>
struct nlohmann::adl_serializer<std::optional<T>> {
    static void from_json(const json & j, std::optional<T>& opt) {
        if(j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
    static void to_json(json & json, std::optional<T> t) {
        if (t) {
            json = *t;
        } else {
            json = nullptr;
        }
    }
};

struct LayerSetup {
    std::vector<double> length;
};

struct LimbSetup {
    // Layer properties
    std::vector<LayerSetup> layers;

    // Geometry at eval points
    std::vector<double> length;
    std::vector<std::array<double, 3>> position;
    std::vector<double> width;
    std::vector<double> height;
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LayerSetup, length)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LimbSetup, length, position, width, height, layers)

struct StateVec {
    std::vector<double> time;
    std::vector<double> draw_length;

    std::vector<std::vector<std::array<double, 3>>> limb_pos;
    std::vector<std::vector<std::array<double, 3>>> limb_vel;
    std::vector<std::vector<std::array<double, 3>>> limb_acc;

    std::vector<std::vector<std::array<double, 2>>> string_pos;
    std::vector<std::vector<std::array<double, 2>>> string_vel;
    std::vector<std::vector<std::array<double, 2>>> string_acc;

    std::vector<std::vector<std::array<double, 3>>> limb_strain;
    std::vector<std::vector<std::array<double, 3>>> limb_force;

    std::vector<std::vector<std::vector<std::tuple<double, double>>>> layer_strain;
    std::vector<std::vector<std::vector<std::tuple<double, double>>>> layer_stress;

    std::vector<double> arrow_pos;
    std::vector<double> arrow_vel;
    std::vector<double> arrow_acc;

    std::vector<double> e_pot_limbs;
    std::vector<double> e_kin_limbs;
    std::vector<double> e_pot_string;
    std::vector<double> e_kin_string;
    std::vector<double> e_kin_arrow;

    std::vector<double> draw_force;
    std::vector<double> grip_force;
    std::vector<double> string_force;
    std::vector<double> strand_force;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    StateVec,
    time,
    draw_length,
    limb_pos,
    limb_vel,
    limb_acc,
    string_pos,
    string_vel,
    string_acc,
    limb_strain,
    limb_force,
    layer_strain,
    layer_stress,
    arrow_pos,
    arrow_vel,
    arrow_acc,
    e_pot_limbs,
    e_kin_limbs,
    e_pot_string,
    e_kin_string,
    e_kin_arrow,
    draw_force,
    grip_force,
    string_force,
    strand_force
)

struct Setup {
    LimbSetup limb;
    double limb_mass;
    double string_mass;
    double string_length;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Setup, limb, limb_mass, string_mass, string_length)

struct Statics {
    StateVec states;

    double final_draw_force;
    double drawing_work;
    double storage_factor;

    std::tuple<double, unsigned> max_string_force;
    std::tuple<double, unsigned> max_grip_force;
    std::tuple<double, unsigned> max_draw_force;

    std::vector<std::tuple<double, unsigned, unsigned>> min_layer_stresses;
    std::vector<std::tuple<double, unsigned, unsigned>> max_layer_stresses;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Statics,
    states,
    final_draw_force,
    drawing_work,
    storage_factor,
    max_string_force,
    max_grip_force,
    max_draw_force,
    min_layer_stresses,
    max_layer_stresses
)


struct Dynamics {
    StateVec states;

    double final_arrow_pos;
    double final_arrow_vel;

    double final_e_kin_arrow;
    double final_e_pot_limbs;
    double final_e_kin_limbs;
    double final_e_pot_string;
    double final_e_kin_string;
    double energy_efficiency;

    std::tuple<double, unsigned> max_string_force;
    std::tuple<double, unsigned> max_grip_force;
    std::tuple<double, unsigned> max_draw_force;

    std::vector<std::tuple<double, unsigned, unsigned>> min_layer_stresses;
    std::vector<std::tuple<double, unsigned, unsigned>> max_layer_stresses;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Dynamics,
    states,
    final_arrow_pos,
    final_arrow_vel,
    final_e_kin_arrow,
    final_e_pot_limbs,
    final_e_kin_limbs,
    final_e_pot_string,
    final_e_kin_string,
    energy_efficiency,
    max_string_force,
    max_grip_force,
    max_draw_force,
    min_layer_stresses,
    max_layer_stresses
)

struct OutputData2 {
    Setup setup;
    std::optional<Statics> statics;
    std::optional<Dynamics> dynamics;

    OutputData2() = default;    // Todo: Make this one obsolete

    OutputData2(const std::string& path);
    void save(const std::string& path) const;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OutputData2, setup, statics, dynamics)
