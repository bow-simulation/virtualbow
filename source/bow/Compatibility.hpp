#pragma once
#include <json.hpp>

using nlohmann::json;

static void convert_0_1_0_to_0_2_0(json& obj)
{
    obj["meta"]["version"] = "0.2";
}

static void convert_0_2_0_to_0_3_0(json& obj)
{
    json obj2;
    obj2["meta"]["version"] = "0.3";
    obj2["meta"]["comments"] = obj["meta"]["comments"];
    obj2["profile"]["segments"] = obj["profile"]["segments"];
    obj2["profile"]["x0"] = obj["profile"]["offset_x"];
    obj2["profile"]["y0"] = obj["profile"]["offset_y"];
    obj2["profile"]["phi0"] = obj["profile"]["angle"];
    obj2["width"] = obj["sections"]["width"];
    obj2["height"] = obj["sections"]["height"];
    obj2["material"]["rho"] = obj["sections"]["rho"];
    obj2["material"]["E"] = obj["sections"]["E"];
    obj2["string"] = obj["string"];
    obj2["masses"] = obj["masses"];
    obj2["operation"] = obj["operation"];
    obj2["settings"]["n_elements_limb"] = obj["settings"]["n_elements_limb"];
    obj2["settings"]["n_elements_string"] = obj["settings"]["n_elements_string"];
    obj2["settings"]["n_draw_steps"] = obj["settings"]["n_draw_steps"];
    obj2["settings"]["time_span_factor"] = obj["settings"]["time_span_factor"];
    obj2["settings"]["time_step_factor"] = obj["settings"]["time_step_factor"];
    obj2["settings"]["sampling_rate"] = 1.0/double(obj["settings"]["sampling_time"]);

    obj = obj2;
}

static void convert_0_3_0_to_0_4_0(json& obj)
{
    json obj2;
    obj2["meta"]["version"] = "0.4";
    obj2["meta"]["comments"] = obj["meta"]["comments"];

    // Reverse curvatures
    std::vector<double> curvatures;
    for(double kappa: obj["profile"]["segments"]["vals"])
        curvatures.push_back(-kappa);

    obj2["profile"]["segments"]["args"] = obj["profile"]["segments"]["args"];
    obj2["profile"]["segments"]["vals"] = curvatures;
    obj2["profile"]["x0"] = obj["profile"]["x0"];
    obj2["profile"]["y0"] = -double(obj["profile"]["y0"]);    // Reverse y-Offset
    obj2["profile"]["phi0"] = -double(obj["profile"]["phi0"]);    // Reverse angular offset
    obj2["width"] = obj["width"];
    obj2["height"] = obj["height"];
    obj2["material"]["rho"] = obj["material"]["rho"];
    obj2["material"]["E"] = obj["material"]["E"];
    obj2["string"]["strand_stiffness"] = obj["string"]["strand_stiffness"];
    obj2["string"]["strand_density"] = obj["string"]["strand_density"];
    obj2["string"]["n_strands"] = obj["string"]["n_strands"];
    obj2["masses"]["string_center"] = obj["masses"]["string_center"];
    obj2["masses"]["string_tip"] = obj["masses"]["string_tip"];
    obj2["masses"]["limb_tip"] = obj["masses"]["limb_tip"];
    obj2["operation"]["brace_height"] = obj["operation"]["brace_height"];
    obj2["operation"]["draw_length"] = obj["operation"]["draw_length"];
    obj2["operation"]["mass_arrow"] = obj["operation"]["mass_arrow"];
    obj2["settings"]["n_elements_limb"] = obj["settings"]["n_elements_limb"];
    obj2["settings"]["n_elements_string"] = obj["settings"]["n_elements_string"];
    obj2["settings"]["n_draw_steps"] = obj["settings"]["n_draw_steps"];
    obj2["settings"]["time_span_factor"] = obj["settings"]["time_span_factor"];
    obj2["settings"]["time_step_factor"] = obj["settings"]["time_step_factor"];
    obj2["settings"]["sampling_rate"] = obj["settings"]["sampling_rate"];

    obj = obj2;
}

static void convert_to_current(json& obj)
{
    if(obj["meta"]["version"] == "0.1" || obj["meta"]["version"] == "0.1.0")
        convert_0_1_0_to_0_2_0(obj);

    if(obj["meta"]["version"] == "0.2" || obj["meta"]["version"] == "0.2.0")
        convert_0_2_0_to_0_3_0(obj);

    if(obj["meta"]["version"] == "0.3" || obj["meta"]["version"] == "0.3.0")
        convert_0_3_0_to_0_4_0(obj);

    if(obj["meta"]["version"] == "0.4" || obj["meta"]["version"] == "0.4.0")
        return;
}
