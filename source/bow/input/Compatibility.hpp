#pragma once
#include <json.hpp>

using nlohmann::json;

static void convert_0_1_0_to_0_2_0(json& obj)
{
    obj.at("meta").at("version") = "0.2";
}

static void convert_0_2_0_to_0_3_0(json& obj)
{
    json obj2;
    obj2.at("meta").at("version") = "0.3";
    obj2.at("meta").at("comments") = obj.at("meta").at("comments");
    obj2.at("profile").at("segments") = obj.at("profile").at("segments");
    obj2.at("profile").at("x0") = obj.at("profile").at("offset_x");
    obj2.at("profile").at("y0") = obj.at("profile").at("offset_y");
    obj2.at("profile").at("phi0") = obj.at("profile").at("angle");
    obj2.at("width") = obj.at("sections").at("width");
    obj2.at("height") = obj.at("sections").at("height");
    obj2.at("material").at("rho") = obj.at("sections").at("rho");
    obj2.at("material").at("E") = obj.at("sections").at("E");
    obj2.at("string") = obj.at("string");
    obj2.at("masses") = obj.at("masses");
    obj2.at("operation") = obj.at("operation");
    obj2.at("settings").at("n_limb_elements") = obj.at("settings").at("n_limb_elements");
    obj2.at("settings").at("n_string_elements") = obj.at("settings").at("n_string_elements");
    obj2.at("settings").at("n_draw_steps") = obj.at("settings").at("n_draw_steps");
    obj2.at("settings").at("time_span_factor") = obj.at("settings").at("time_span_factor");
    obj2.at("settings").at("time_step_factor") = obj.at("settings").at("time_step_factor");
    obj2.at("settings").at("sampling_rate") = 1.0/double(obj.at("settings").at("sampling_time"));

    obj = obj2;
}

static void convert_0_3_0_to_0_4_0(json& obj)
{
    json obj2;
    obj2.at("meta").at("version") = "0.4";
    obj2.at("meta").at("comments") = obj.at("meta").at("comments");

    // Reverse curvatures
    std::vector<double> curvatures;
    for(double kappa: obj.at("profile").at("segments").at("vals"))
        curvatures.push_back(-kappa);

    obj2.at("profile").at("segments").at("args") = obj.at("profile").at("segments").at("args");
    obj2.at("profile").at("segments").at("vals") = curvatures;
    obj2.at("profile").at("x0") = obj.at("profile").at("x0");
    obj2.at("profile").at("y0") = -double(obj.at("profile").at("y0"));    // Reverse y-Offset
    obj2.at("profile").at("phi0") = -double(obj.at("profile").at("phi0"));    // Reverse angular offset
    obj2.at("width") = obj.at("width");
    obj2.at("height") = obj.at("height");
    obj2.at("material").at("rho") = obj.at("material").at("rho");
    obj2.at("material").at("E") = obj.at("material").at("E");
    obj2.at("string").at("strand_stiffness") = obj.at("string").at("strand_stiffness");
    obj2.at("string").at("strand_density") = obj.at("string").at("strand_density");
    obj2.at("string").at("n_strands") = obj.at("string").at("n_strands");
    obj2.at("masses").at("string_center") = obj.at("masses").at("string_center");
    obj2.at("masses").at("string_tip") = obj.at("masses").at("string_tip");
    obj2.at("masses").at("limb_tip") = obj.at("masses").at("limb_tip");
    obj2.at("operation").at("brace_height") = obj.at("operation").at("brace_height");
    obj2.at("operation").at("draw_length") = obj.at("operation").at("draw_length");
    obj2.at("operation").at("arrow_mass") = obj.at("operation").at("arrow_mass");
    obj2.at("settings").at("n_limb_elements") = obj.at("settings").at("n_limb_elements");
    obj2.at("settings").at("n_string_elements") = obj.at("settings").at("n_string_elements");
    obj2.at("settings").at("n_draw_steps") = obj.at("settings").at("n_draw_steps");
    obj2.at("settings").at("time_span_factor") = obj.at("settings").at("time_span_factor");
    obj2.at("settings").at("time_step_factor") = obj.at("settings").at("time_step_factor");
    obj2.at("settings").at("sampling_rate") = obj.at("settings").at("sampling_rate");

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
        return;
}
