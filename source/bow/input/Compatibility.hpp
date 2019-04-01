#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

static void convert_0_1_0_to_0_2_0(json& obj)
{
    obj["meta"]["version"] = "0.2";
}

static void convert_0_2_0_to_0_3_0(json& obj)
{
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

static void convert_0_3_0_to_0_4_0(json& obj)
{
    json obj2;
    obj2["meta"]["version"] = "0.4";
    obj2["meta"]["comments"] = obj.at("meta").at("comments");

    // Reverse curvatures
    std::vector<double> curvatures;
    for(double kappa: obj.at("profile").at("segments").at("vals"))
        curvatures.push_back(-kappa);

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

static void convert_0_4_0_to_0_5_0(json& obj)
{
    auto series_to_array = [](const json& input)
    {
        json output;
        for(size_t i = 0; i < input.at("args").size(); ++i)
        {
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

static void convert_to_current(json& obj)
{
    if(obj.at("meta").at("version") == "0.1" || obj.at("meta").at("version") == "0.1.0")
        convert_0_1_0_to_0_2_0(obj);

    if(obj.at("meta").at("version") == "0.2" || obj.at("meta").at("version") == "0.2.0")
        convert_0_2_0_to_0_3_0(obj);

    if(obj.at("meta").at("version") == "0.3" || obj.at("meta").at("version") == "0.3.0")
        convert_0_3_0_to_0_4_0(obj);

    if(obj.at("meta").at("version") == "0.4" || obj.at("meta").at("version") == "0.4.0")
        convert_0_4_0_to_0_5_0(obj);

    if(obj.at("meta").at("version") == "0.5" || obj.at("meta").at("version") == "0.5.0")
        return;

    throw std::runtime_error("Version not recognized.");
}
