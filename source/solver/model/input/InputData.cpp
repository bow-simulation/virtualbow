#include "InputData.hpp"
#include "Conversion.hpp"
#include <fstream>
#include <iomanip>

InputData::InputData(const std::string& path) {
    std::ifstream stream(path);
    json obj; stream >> obj;

    Conversion::to_current(obj);
    from_json(obj, *this);
}

void InputData::save(const std::string& path) const {
    json obj;
    to_json(obj, *this);

    std::ofstream stream(path);
    stream << std::setw(4) << obj << std::endl;
}

std::string InputData::validate() const {
    // Check Settings
    if(settings.n_limb_elements < 1) {
        return "Settings: Number of limb elements must be positive";
    }
    if(settings.n_string_elements < 1) {
        return "Settings: Number of string elements must be positive";
    }
    if(settings.n_draw_steps < 1) {
        return "Settings: Number of draw steps must be positive";
    }
    if(settings.time_span_factor <= 0.0) {
        return "Settings: Time span factor must be positive";
    }
    if(settings.time_step_factor <= 0.0) {
        return "Settings: Time step factor must be positive";
    }
    if(settings.sampling_rate <= 0.0) {
        return "Settings: Sampling rate must be positive";
    }
    if(settings.arrow_clamp_force < 0.0) {
        return "Settings: Arrow clamp force must not be negative";
    }

    // Check Profile
    // ...

    // Check Width
    if(width.size() < 2) {
        return "Width: At least two data points are needed";
    }
    for(double w: width.col(1)) {
        if(w <= 0.0) {
            return "Width: All width values must be positive";
        }
    }

    // Check Layers
    for(size_t i = 0; i < layers.size(); ++i) {
        const Layer& layer = layers[i];

        if(layer.rho <= 0.0) {
            return "Layer " + std::to_string(i) + " (" + layer.name + ")" + ": rho must be positive";
        }
        if(layer.E <= 0.0) {
            return "Layer " + std::to_string(i) + " (" + layer.name + ")" + ": E must be positive";
        }
        if(layer.height.size() < 2) {
            return "Layer " + std::to_string(i) + " (" + layer.name + ")" + ": At least two data points for height are needed";
        }
        for(double h: layer.height.col(1)) {
            if(h < 0.0) {
                return "Layer " + std::to_string(i) + " (" + layer.name + ")" + ": Height must not be negative";
            }
        }
    }

    // Check String
    if(string.strand_stiffness <= 0.0) {
        return "String: Strand stiffness must be positive";
    }
    if(string.strand_density <= 0.0) {
        return "String: Strand density must be positive";
    }
    if(string.n_strands < 1) {
        return "String: Number of strands must be positive";
    }

    // Check Masses
    if(masses.arrow <= 0.0) {
        return "Operation: Arrow mass must be positive";
    }
    if(masses.string_center < 0.0) {
        return "Masses: String center mass must not be negative";
    }
    if(masses.string_tip < 0.0) {
        return "Masses: String tip mass must not be negative";
    }
    if(masses.limb_tip < 0.0) {
        return "Masses: Limb tip mass must not be negative";
    }

    // Check Damping
    if(damping.damping_ratio_limbs < 0.0 || damping.damping_ratio_limbs > 1.0) {
        return "Damping: Damping of the limb must be 0% ... 100%";
    }
    if(damping.damping_ratio_string < 0.0 || damping.damping_ratio_string > 1.0) {
        return "Damping: Damping of the string must be 0% ... 100%";
    }

    // Check Dimensions
    if(dimensions.brace_height >= dimensions.draw_length) {
        return "Dimensions: Draw length must be greater than brace height";
    }
    if(dimensions.handle_length < 0.0) {
        return "Dimensions: Handle length must be positive";
    }

    return std::string();
}
