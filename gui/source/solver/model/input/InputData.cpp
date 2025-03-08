#include "InputData.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>

using nlohmann::json;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, n_limb_elements, n_limb_eval_points, min_draw_resolution, max_draw_resolution, arrow_clamp_force, string_compression_factor, timespan_factor, timeout_factor, min_timestep, max_timestep, steps_per_period)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Material, name, color, rho, E)
NLOHMANN_JSON_SERIALIZE_ENUM(HandleOrigin, {{HandleOrigin::Back, "back"}, {HandleOrigin::Belly, "belly"}, {HandleOrigin::Profile, "profile"}})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Dimensions, brace_height, draw_length, handle_origin, handle_length, handle_offset, handle_angle)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Layer, name, material, height)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Masses, arrow, string_center, string_tip, limb_tip)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Damping, damping_ratio_limbs, damping_ratio_string)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(String, strand_stiffness, strand_density, n_strands)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputData, comment, settings, materials, dimensions, profile, width, layers, string, masses, damping)

InputData::InputData(const std::string& path) {
    std::ifstream stream(path);
    stream.exceptions(~std::ofstream::goodbit);    // Make stream throw exception on failure

    json obj; stream >> obj;
    from_json(obj, *this);
}

void InputData::save(const std::string& path) const {
    json obj;
    to_json(obj, *this);

    std::ofstream stream(path);
    stream.exceptions(~std::ofstream::goodbit);    // Make stream throw exception on failure
    stream << std::setw(4) << obj << std::endl;
}
