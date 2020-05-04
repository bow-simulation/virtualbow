#pragma once
#include "solver/model/output/BowStates.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct DynamicData
{
    BowStates states;

    double final_pos_arrow = 0;
    double final_vel_arrow = 0;

    double final_e_pot_limbs = 0;
    double final_e_kin_limbs = 0;
    double final_e_pot_string = 0;
    double final_e_kin_string = 0;
    double final_e_kin_arrow = 0;

    double efficiency = 0.0;

    unsigned max_string_force_index = 0;
    unsigned max_grip_force_index = 0;
    unsigned arrow_departure_index = 0;

    std::vector<double> max_stress_value;
    std::vector<std::pair<unsigned, unsigned>> max_stress_index;
};

static void to_json(json& obj, const DynamicData& val)
{
    obj["states"] = val.states;
    obj["final_pos_arrow"] =  val.final_pos_arrow;
    obj["final_vel_arrow"] =  val.final_vel_arrow;
    obj["final_e_pot_limbs"] =  val.final_e_pot_limbs;
    obj["final_e_kin_limbs"] =  val.final_e_kin_limbs;
    obj["final_e_pot_string"] =  val.final_e_pot_string;
    obj["final_e_kin_string"] =  val.final_e_kin_string;
    obj["final_e_kin_arrow"] =  val.final_e_kin_arrow;
    obj["efficiency"] =  val.efficiency;
    obj["max_string_force_index"] = val.max_string_force_index;
    obj["max_grip_force_index"] = val.max_grip_force_index;
    obj["max_stress_value"] = val.max_stress_value;
    obj["max_stress_index"] = val.max_stress_index;
}

static void from_json(const json& obj, DynamicData& val)
{
    val.states = obj.at("states");
    val.final_pos_arrow = obj.at("final_pos_arrow");
    val.final_vel_arrow = obj.at("final_vel_arrow");
    val.final_e_pot_limbs = obj.at("final_e_pot_limbs");
    val.final_e_kin_limbs = obj.at("final_e_kin_limbs");
    val.final_e_pot_string = obj.at("final_e_pot_string");
    val.final_e_kin_string = obj.at("final_e_kin_string");
    val.final_e_kin_arrow = obj.at("final_e_kin_arrow");
    val.efficiency = obj.at("efficiency");
    val.max_string_force_index = obj.at("max_string_force_index");
    val.max_grip_force_index = obj.at("max_grip_force_index");
    val.max_stress_value = obj.at("max_stress_value").get<std::vector<double>>();
    val.max_stress_index = obj.at("max_stress_index").get<std::vector<std::pair<unsigned, unsigned>>>();
}
