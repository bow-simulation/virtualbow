#include "OutputData.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using nlohmann::json;

void OutputData::save(const std::string& path) const
{
    // https://github.com/nlohmann/json/issues/479
    std::vector<uint8_t> buffer = json::to_msgpack(*this);
    std::ofstream stream(path, std::ios::out | std::ios::binary);
    stream.exceptions(~std::ofstream::goodbit);    // Make stream throw exception on failure
    stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

OutputData::OutputData(const std::string& path)
{
    // https://github.com/nlohmann/json/issues/479
    std::ifstream stream(path, std::ios::binary);
    stream.exceptions(~std::ofstream::goodbit);    // Make stream throw exception on failure
    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    json obj = json::from_msgpack(buffer);
    from_json(obj, *this);
}

// Todo: Remove computations from here
OutputData::OutputData(SetupData setup, BowStates static_states, BowStates dynamic_states)
    : setup(setup)
{
    // Helper functions

    auto abs_max_order = [](double a, double b) {
        return std::abs(a) < std::abs(b);
    };

    auto max_abs_force = [&](const std::vector<double>& vec) {
        return std::max_element(vec.begin(), vec.end(), abs_max_order) - vec.begin();
    };

    auto min_max_stress = [&](const BowStates& states,
                            std::vector<double>& min_stress_value, std::vector<std::pair<unsigned, unsigned>>& min_stress_index,
                            std::vector<double>& max_stress_value, std::vector<std::pair<unsigned, unsigned>>& max_stress_index)
    {
        for(const LayerProperties& layer: setup.limb_properties.layers) {
            double sigma_min = std::numeric_limits<double>::max();
            std::pair<unsigned, unsigned> index_min = {0, 0};

            double sigma_max = std::numeric_limits<double>::lowest();
            std::pair<unsigned, unsigned> index_max = {0, 0};

            for(size_t i = 0; i < states.time.size(); ++i) {
                VectorXd sigma_back = layer.He_back * states.epsilon[i] + layer.Hk_back * states.kappa[i];
                VectorXd sigma_belly = layer.He_belly * states.epsilon[i] + layer.Hk_belly * states.kappa[i];

                for(int j = 0; j < layer.length.size(); ++j) {
                    if(sigma_back[j] > sigma_max) {
                        sigma_max = sigma_back[j];
                        index_max = {i, j};
                    }
                    if(sigma_back[j] < sigma_min) {
                        sigma_min = sigma_back[j];
                        index_min = {i, j};
                    }
                    if(sigma_belly[j] > sigma_max) {
                        sigma_max = sigma_belly[j];
                        index_max = {i, j};
                    }
                    if(sigma_belly[j] < sigma_min) {
                        sigma_min = sigma_belly[j];
                        index_min = {i, j};
                    }
                }
            }

            min_stress_value.push_back(sigma_min);
            min_stress_index.push_back(index_min);

            max_stress_value.push_back(sigma_max);
            max_stress_index.push_back(index_max);
        }
    };

    // Assign bow states
    statics.states = static_states;
    dynamics.states = dynamic_states;

    // Calculate static numbers
    if(!statics.states.time.empty()) {
        double draw_length_front = static_states.draw_length.front();
        double draw_length_back = static_states.draw_length.back();
        double draw_force_back = static_states.draw_force.back();
        double e_pot_front = static_states.e_pot_limbs.front() + static_states.e_pot_string.front();
        double e_pot_back = static_states.e_pot_limbs.back() + static_states.e_pot_string.back();

        statics.final_draw_force = draw_force_back;
        statics.drawing_work = e_pot_back - e_pot_front;
        statics.energy_storage_factor = (e_pot_back - e_pot_front)/(0.5*(draw_length_back - draw_length_front)*draw_force_back);
        statics.max_string_force_index = max_abs_force(static_states.string_force);
        statics.max_grip_force_index = max_abs_force(static_states.grip_force);
        statics.max_draw_force_index = max_abs_force(static_states.draw_force);

        min_max_stress(static_states, statics.min_stress_value, statics.min_stress_index, statics.max_stress_value, statics.max_stress_index);
    }

    // Calculate dynamic numbers
    if(!dynamics.states.time.empty()) {
        for(size_t i = 0; i < dynamic_states.time.size(); ++i) {
            // Find point of arrow separation
            if(dynamic_states.acc_arrow[i] == 0.0) {
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
        dynamics.max_string_force_index = max_abs_force(dynamic_states.string_force);
        dynamics.max_grip_force_index = max_abs_force(dynamic_states.grip_force);

        min_max_stress(dynamic_states, dynamics.min_stress_value, dynamics.min_stress_index, dynamics.max_stress_value, dynamics.max_stress_index);
    }
}

