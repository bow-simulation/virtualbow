#include "Conversion.hpp"
#include <cmath>

void Conversion::to_current(json& obj) {
    if(obj.contains("meta") && obj.at("meta").at("version") == "0.1") {
        convert_0_1_0_to_0_2_0(obj);
    }

    if(obj.contains("meta") && obj.at("meta").at("version") == "0.2") {
        convert_0_2_0_to_0_3_0(obj);
    }

    if(obj.contains("meta") && obj.at("meta").at("version") == "0.3") {
        convert_0_3_0_to_0_4_0(obj);
    }

    if(obj.contains("meta") && obj.at("meta").at("version") == "0.4") {
        convert_0_4_0_to_0_5_0(obj);
    }

    if(obj.contains("meta") && obj.at("meta").at("version") == "0.5") {
        convert_0_5_0_to_0_6_0(obj);
    }

    if(obj.contains("meta") && obj.at("meta").at("version") == "0.6") {
        convert_0_6_0_to_0_6_1(obj);
    }

    if(obj.contains("meta") && obj.at("meta").at("version") == "0.6.1") {
        convert_0_6_1_to_0_7_0(obj);
    }

    if(obj.contains("version") && obj.at("version") == "0.7") {
        convert_0_7_0_to_0_7_1(obj);
    }

    if(obj.contains("version") && obj.at("version") == "0.7.1") {
        convert_0_7_1_to_0_8_0(obj);
    }

    if(obj.contains("version") && obj.at("version") == "0.8") {
        convert_0_8_0_to_0_9_0(obj);
    }

    if(obj.contains("version") && obj.at("version") == "0.9") {
        convert_0_9_0_to_0_9_1(obj);
    }

    if(obj.contains("version") && obj.at("version") == "0.9.1") {
        return;
    }

    throw std::runtime_error("Version not recognized.");
}

void Conversion::convert_0_9_0_to_0_9_1(json& obj) {
    obj["version"] = "0.9.1";
}

void Conversion::convert_0_8_0_to_0_9_0(json& obj) {
    obj["version"] = "0.9";

    // In previous versions, the colors were ransomly generated based on the material properties (same properties -> same color)
    // Starting with version 0.9 the colors can be chosen by users, so we have to pick some initial color here. Instead of replicating the old
    // random algorithm, they are chosen out of a fixed color palette (taken from Python's Matplotlib,  https://stackoverflow.com/a/42091037)
    const std::array<std::string, 10> color_palette = { "#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf" };
    std::map<double, std::string> used_colors;

    int next_color = 0;
    auto material_color = [&](double rho, double E) {
        // Check if the material (rho, E) was already assigned a color
        // If not, assign it the next color in the palette (with wrap-around)
        double key = rho*E;
        auto it = used_colors.find(key);
        if(it == used_colors.end()) {
            used_colors[key] = color_palette[next_color % color_palette.size()];
            next_color += 1;
        }

        return used_colors[key];
    };

    // Create new material from each layer and make layer refer to that material
    // Also name material after layer and give layers a generic name instead
    obj["materials"] = json::array();
    for(size_t i = 0; i < obj.at("layers").size(); ++i) {
        json& layer = obj.at("layers").at(i);

        obj["materials"].push_back({
            {"name", layer.at("name")},
            {"color", material_color(layer.at("rho"), layer.at("E"))},
            {"rho", layer.at("rho")},
            {"E", layer.at("E")}
        });

        layer["name"] = "Layer " + std::to_string(i+1);
        layer["material"] = i;
        layer.erase("rho");
        layer.erase("E");
    }

    json old_profile = obj.at("profile");
    json new_profile = json::array();

    auto curvature_to_radius = [](double kappa) {
        return (kappa != 0.0) ? 1.0/kappa : 0.0;
    };

    // Convert old profile definition to line, arc and spiral segments
    for(size_t i = 0; i < old_profile.size() - 1; ++i) {
        json prev = old_profile.at(i);
        json next = old_profile.at(i+1);

        double length = next.at(0).get<double>() - prev.at(0).get<double>();
        double kappa0 = prev.at(1).get<double>();
        double kappa1 = next.at(1).get<double>();

        if(kappa0 == 0.0 && kappa1 == 0.0) {
            new_profile.push_back({
                {"parameters", {
                     {"length", length}
                }},
                {"type", "line"}
            });
        }
        else if(kappa0 == kappa1) {
            new_profile.push_back({
                {"parameters", {
                     {"length", length},
                     {"radius", curvature_to_radius(kappa0)}
                }},
                {"type", "arc"}
            });
        }
        else {
            new_profile.push_back({
                {"parameters", {
                     {"length", length},
                     {"r_start", curvature_to_radius(kappa0)},
                     {"r_end", curvature_to_radius(kappa1)}
                }},
                {"type", "spiral"}
            });
        }
    }

    obj["profile"] = new_profile;
}

void Conversion::convert_0_7_1_to_0_8_0(json& obj) {
    obj["version"] = "0.8";
    obj["settings"]["arrow_clamp_force"] = 0.0;
}

void Conversion::convert_0_7_0_to_0_7_1(json& obj) {
    obj["version"] = "0.7.1";
}

void Conversion::convert_0_6_1_to_0_7_0(json& obj) {
    obj["version"] = "0.7";
    obj["comment"] = obj["meta"]["comments"];
    obj.erase("meta");

    obj["damping"]["damping_ratio_limbs"] = 0.0;
    obj["damping"]["damping_ratio_string"] = 0.0;

    // Old description of the profile curve: List of segments {seg length, curvature}, the curvature is constant within each segment.
    // New description: List of points {arc length, curvature}, the curvature is interpolated linearly between points.
    // With the new description, no jumps in curvature are possible anymore. Therefore insert a small linear transition between areas of constant curvature.

    // Ratio of the segment length used as transition between constant curvatures
    const double epsilon = 0.01;

    json old_profile = obj.at("profile");
    json new_profile = json::array();

    double s0 = 0.0;
    for(size_t i = 0; i < old_profile.size(); ++i) {
        json section = old_profile.at(i);
        double s1 = s0 + section.at(0).get<double>();
        double k = section.at(1).get<double>();
        double d = epsilon*(s1 - s0);

        if(i == 0) {
            // Section start point without transition
            new_profile.push_back(json::array());
            new_profile.back().push_back(s0);
            new_profile.back().push_back(k);
        }
        else {
            // Section start point with transition
            new_profile.push_back(json::array());
            new_profile.back().push_back(s0 + d);
            new_profile.back().push_back(k);
        }

        if(i == old_profile.size() - 1) {
            // Section end point without transition
            new_profile.push_back(json::array());
            new_profile.back().push_back(s1);
            new_profile.back().push_back(k);
        }
        else {
            // Section end point with transition
            new_profile.push_back(json::array());
            new_profile.back().push_back(s1 - d);
            new_profile.back().push_back(k);
        }

        s0 = s1;
    }

    obj["profile"] = new_profile;
}

void Conversion::convert_0_6_0_to_0_6_1(json& obj) {
    obj["meta"]["version"] = "0.6.1";
}

void Conversion::convert_0_5_0_to_0_6_0(json& obj) {
    json obj2;
    obj2["meta"]["version"] = "0.6";
    obj2["meta"]["comments"] = obj.at("meta").at("comments");
    obj2["settings"] = obj.at("settings");
    obj2["profile"] = obj.at("profile").at("segments");
    obj2["width"] = obj.at("width");
    obj2["layers"] = obj.at("layers");
    obj2["dimensions"]["brace_height"] = obj.at("operation").at("brace_height");
    obj2["dimensions"]["draw_length"] = obj.at("operation").at("draw_length");
    obj2["dimensions"]["handle_angle"] = obj.at("profile").at("angle");
    obj2["dimensions"]["handle_length"] = 2.0*std::abs((double) obj.at("profile").at("x_pos"));
    obj2["dimensions"]["handle_setback"] = obj.at("profile").at("y_pos");
    obj2["string"] = obj.at("string");
    obj2["masses"]["arrow"] = obj.at("operation").at("arrow_mass");
    obj2["masses"]["limb_tip"] = obj.at("masses").at("limb_tip");
    obj2["masses"]["string_center"] = obj.at("masses").at("string_center");
    obj2["masses"]["string_tip"] = obj.at("masses").at("string_tip");
    obj = obj2;
}

void Conversion::convert_0_4_0_to_0_5_0(json& obj) {
    auto series_to_array = [](const json& input) {
        json output;
        for(size_t i = 0; i < input.at("args").size(); ++i) {
            output.push_back(json::array());
            output[i].push_back(input.at("args").at(i));
            output[i].push_back(input.at("vals").at(i));
        }

        return output;
    };

    json obj2;
    obj2["meta"]["version"] = "0.5";
    obj2["meta"]["comments"] = obj.at("meta").at("comments");
    obj2["settings"]["n_draw_steps"] = obj.at("settings").at("n_draw_steps");
    obj2["settings"]["n_limb_elements"] = obj.at("settings").at("n_elements_limb");
    obj2["settings"]["n_string_elements"] = obj.at("settings").at("n_elements_string");
    obj2["settings"]["sampling_rate"] = obj.at("settings").at("sampling_rate");
    obj2["settings"]["time_span_factor"] = obj.at("settings").at("time_span_factor");
    obj2["settings"]["time_step_factor"] = obj.at("settings").at("time_step_factor");
    obj2["profile"]["segments"] = series_to_array(obj.at("profile").at("segments"));
    obj2["profile"]["angle"] = obj.at("profile").at("phi0");
    obj2["profile"]["x_pos"] = obj.at("profile").at("x0");
    obj2["profile"]["y_pos"] = obj.at("profile").at("y0");
    obj2["width"] = series_to_array(obj.at("width"));
    obj2["layers"].push_back(json());
    obj2["layers"][0]["name"] = "unnamed";
    obj2["layers"][0]["height"] = series_to_array(obj.at("height"));
    obj2["layers"][0]["rho"] = obj.at("material").at("rho");
    obj2["layers"][0]["E"] = obj.at("material").at("E");
    obj2["string"] = obj.at("string");
    obj2["masses"] = obj.at("masses");
    obj2["operation"]["arrow_mass"] = obj.at("operation").at("mass_arrow");
    obj2["operation"]["brace_height"] = obj.at("operation").at("brace_height");
    obj2["operation"]["draw_length"] = obj.at("operation").at("draw_length");

    obj = obj2;
}

void Conversion::convert_0_3_0_to_0_4_0(json& obj) {
    json obj2;
    obj2["meta"]["version"] = "0.4";
    obj2["meta"]["comments"] = obj.at("meta").at("comments");

    // Reverse curvatures
    std::vector<double> curvatures;
    for(double kappa: obj.at("profile").at("segments").at("vals")) {
        curvatures.push_back(-kappa);
    }

    obj2["profile"]["segments"]["args"] = obj.at("profile").at("segments").at("args");
    obj2["profile"]["segments"]["vals"] = curvatures;
    obj2["profile"]["x0"] = obj.at("profile").at("x0");
    obj2["profile"]["y0"] = -double(obj.at("profile").at("y0"));    // Reverse y-Offset
    obj2["profile"]["phi0"] = -double(obj.at("profile").at("phi0"));    // Reverse angular offset
    obj2["width"] = obj.at("width");
    obj2["height"] = obj.at("height");
    obj2["material"]["rho"] = obj.at("material").at("rho");
    obj2["material"]["E"] = obj.at("material").at("E");
    obj2["string"]["strand_stiffness"] = obj.at("string").at("strand_stiffness");
    obj2["string"]["strand_density"] = obj.at("string").at("strand_density");
    obj2["string"]["n_strands"] = obj.at("string").at("n_strands");
    obj2["masses"]["string_center"] = obj.at("masses").at("string_center");
    obj2["masses"]["string_tip"] = obj.at("masses").at("string_tip");
    obj2["masses"]["limb_tip"] = obj.at("masses").at("limb_tip");
    obj2["operation"]["brace_height"] = obj.at("operation").at("brace_height");
    obj2["operation"]["draw_length"] = obj.at("operation").at("draw_length");
    obj2["operation"]["arrow_mass"] = obj.at("operation").at("arrow_mass");
    obj2["settings"]["n_limb_elements"] = obj.at("settings").at("n_limb_elements");
    obj2["settings"]["n_string_elements"] = obj.at("settings").at("n_string_elements");
    obj2["settings"]["n_draw_steps"] = obj.at("settings").at("n_draw_steps");
    obj2["settings"]["time_span_factor"] = obj.at("settings").at("time_span_factor");
    obj2["settings"]["time_step_factor"] = obj.at("settings").at("time_step_factor");
    obj2["settings"]["sampling_rate"] = obj.at("settings").at("sampling_rate");

    obj = obj2;
}

void Conversion::convert_0_2_0_to_0_3_0(json& obj) {
    json obj2;
    obj2["meta"]["version"] = "0.3";
    obj2["meta"]["comments"] = obj.at("meta").at("comments");
    obj2["profile"]["segments"] = obj.at("profile").at("segments");
    obj2["profile"]["x0"] = obj.at("profile").at("offset_x");
    obj2["profile"]["y0"] = obj.at("profile").at("offset_y");
    obj2["profile"]["phi0"] = obj.at("profile").at("angle");
    obj2["width"] = obj.at("sections").at("width");
    obj2["height"] = obj.at("sections").at("height");
    obj2["material"]["rho"] = obj.at("sections").at("rho");
    obj2["material"]["E"] = obj.at("sections").at("E");
    obj2["string"] = obj.at("string");
    obj2["masses"] = obj.at("masses");
    obj2["operation"] = obj.at("operation");
    obj2["settings"]["n_limb_elements"] = obj.at("settings").at("n_limb_elements");
    obj2["settings"]["n_string_elements"] = obj.at("settings").at("n_string_elements");
    obj2["settings"]["n_draw_steps"] = obj.at("settings").at("n_draw_steps");
    obj2["settings"]["time_span_factor"] = obj.at("settings").at("time_span_factor");
    obj2["settings"]["time_step_factor"] = obj.at("settings").at("time_step_factor");
    obj2["settings"]["sampling_rate"] = 1.0/double(obj.at("settings").at("sampling_time"));

    obj = obj2;
}

void Conversion::convert_0_1_0_to_0_2_0(json& obj) {
    obj["meta"]["version"] = "0.2";
}
