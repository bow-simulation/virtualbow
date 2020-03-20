#pragma once
#include "bow/output/BowStates.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct DynamicData
{
    BowStates states;

    double final_pos_arrow;
    double final_vel_arrow;

    double final_e_pot_limbs;
    double final_e_kin_limbs;
    double final_e_pot_string;
    double final_e_kin_string;
    double final_e_kin_arrow;

    double efficiency = 0.0;

    double max_string_force = 0.0;
    double max_strand_force = 0.0;
    double max_grip_force = 0.0;
    double max_draw_force = 0.0;
    std::vector<double> max_stress_pos;
    std::vector<double> max_stress_val;
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
    obj["max_string_force"] = val.max_string_force;
    obj["max_strand_force"] = val.max_strand_force;
    obj["max_grip_force"] = val.max_grip_force;
    obj["max_draw_force"] = val.max_draw_force;
    obj["max_stress_pos"] = val.max_stress_pos;
    obj["max_stress_val"] = val.max_stress_val;
}
