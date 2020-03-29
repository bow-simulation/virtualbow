#pragma once
#include "bow/output/SetupData.hpp"
#include "bow/output/StaticData.hpp"
#include "bow/output/DynamicData.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;

struct OutputData
{
    SetupData setup;
    StaticData statics;
    DynamicData dynamics;

    // Todo: Make this one obsolete
    OutputData() = default;

    OutputData(SetupData setup, BowStates static_states, BowStates dynamic_states)
        : setup(setup)
    {
        // Helper functions

        auto absMaxOrder = [](double a, double b) {
            return std::abs(a) < std::abs(b);
        };

        auto maxAbsForce = [&](const std::vector<double>& vec) {
            return std::max_element(vec.begin(), vec.end(), absMaxOrder) - vec.begin();
        };

        auto maxAbsStress = [&](const BowStates& states, std::vector<double>& max_stress_value, std::vector<std::pair<unsigned, unsigned>>& max_stress_index)
        {
            for(const LayerProperties& layer: setup.limb_properties.layers)
            {
                double sigma_max = 0.0;
                std::pair<unsigned, unsigned> index = {0, 0};

                for(int i = 0; i < states.time.size(); ++i)
                {
                    VectorXd sigma_back = layer.He_back * states.epsilon[i] + layer.Hk_back * states.kappa[i];
                    VectorXd sigma_belly = layer.He_belly * states.epsilon[i] + layer.Hk_belly * states.kappa[i];

                    for(int j = 0; j < layer.length.size(); ++j)
                    {
                        if(std::abs(sigma_back[j]) > std::abs(sigma_max)) {
                            sigma_max = sigma_back[j];
                            index = {i, j};
                        }

                        if(std::abs(sigma_belly[j]) > std::abs(sigma_max)) {
                            sigma_max = sigma_belly[j];
                            index = {i, j};
                        }
                    }
                }

                max_stress_value.push_back(sigma_max);
                max_stress_index.push_back(index);
            }
        };

        // Assign bow states
        statics.states = static_states;
        dynamics.states = dynamic_states;

        // Calculate static numbers
        if(!statics.states.time.empty())
        {
            double draw_length_front = static_states.draw_length.front();
            double draw_length_back = static_states.draw_length.back();
            double draw_force_back = static_states.draw_force.back();
            double e_pot_front = static_states.e_pot_limbs.front() + static_states.e_pot_string.front();
            double e_pot_back = static_states.e_pot_limbs.back() + static_states.e_pot_string.back();

            statics.final_draw_force = draw_force_back;
            statics.drawing_work = e_pot_back - e_pot_front;
            statics.storage_ratio = (e_pot_back - e_pot_front)/(0.5*(draw_length_back - draw_length_front)*draw_force_back);
            statics.max_string_force_index = maxAbsForce(static_states.string_force);
            statics.max_grip_force_index = maxAbsForce(static_states.grip_force);
            statics.max_draw_force_index = maxAbsForce(static_states.draw_force);
            maxAbsStress(static_states, statics.max_stress_value, statics.max_stress_index);
        }

        // Calculate dynamic numbers
        if(!dynamics.states.time.empty())
        {
            for(int i = 0; i < dynamic_states.time.size(); ++i)
            {
                // Find point of arrow separation
                if(dynamic_states.acc_arrow[i] <= 0.0)
                {
                    dynamics.final_pos_arrow = dynamic_states.pos_arrow[i];
                    dynamics.final_vel_arrow = dynamic_states.vel_arrow[i];
                    dynamics.final_e_pot_limbs = dynamic_states.e_pot_limbs[i];
                    dynamics.final_e_kin_limbs = dynamic_states.e_kin_limbs[i];
                    dynamics.final_e_pot_string = dynamic_states.e_pot_string[i];
                    dynamics.final_e_kin_string = dynamic_states.e_kin_string[i];
                    dynamics.final_e_kin_arrow = dynamic_states.e_kin_arrow[i];
                    dynamics.arrow_departure_index = i;
                    break;
                }
            }

            dynamics.efficiency = dynamics.final_e_kin_arrow/statics.drawing_work;
            dynamics.max_string_force_index = maxAbsForce(dynamic_states.string_force);
            dynamics.max_grip_force_index = maxAbsForce(dynamic_states.grip_force);
            maxAbsStress(dynamic_states, dynamics.max_stress_value, dynamics.max_stress_index);
        }
    }

    // Todo: Move this somewhere else, maybe in Application class. Interface of OutputData can be just json.
    void save(const std::string& path) const;
};

static void to_json(json& obj, const OutputData& val)
{
    obj["setup"] = val.setup;
    obj["statics"] = val.statics;
    obj["dynamics"] = val.dynamics;
}
