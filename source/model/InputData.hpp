#pragma once
#include "Document.hpp"
#include "../numerics/Series.hpp"
#include <jsoncons/json.hpp>
#include <fstream>


struct InputData: public Document
{
    // Meta
    DocItem<std::string>  meta_version{"", this};
    DocItem<std::string> meta_comments{"", this};

    // Profile
    DocItem<Series> profile_curvature{{{0.3, 0.4, 0.2}, {-1.0, 1.0, -2.0}}, this};
    DocItem<double> profile_offset_x{0.0, this};
    DocItem<double> profile_offset_y{0.0, this};
    DocItem<double>    profile_angle{0.0, this};

    // Sections
    DocItem<Series>  sections_width{{{0.0, 1.0}, {0.01, 0.008}}, this};
    DocItem<Series> sections_height{{{0.0, 1.0}, {0.01, 0.008}}, this};
    DocItem<double> sections_rho { 500.0, this, &Domain<double>::pos};
    DocItem<double>    sections_E{40.0e9, this, &Domain<double>::pos};

    // String
    DocItem<double> string_strand_stiffness{3100.0, this, &Domain<double>::pos};
    DocItem<double>   string_strand_density{  0.05, this, &Domain<double>::pos};
    DocItem<double>        string_n_strands{  10.0, this, &Domain<double>::pos};

    // Operation
    DocItem<double> operation_brace_height{ 0.2, this};
    DocItem<double>  operation_draw_length{ 0.7, this, &Domain<double>::pos};
    DocItem<double>   operation_mass_arrow{0.02, this, &Domain<double>::pos};

    // Additional masses
    DocItem<double> mass_string_center{0.0, this, &Domain<double>::non_neg};
    DocItem<double>    mass_string_tip{0.0, this, &Domain<double>::non_neg};
    DocItem<double>      mass_limb_tip{0.0, this, &Domain<double>::non_neg};

    // Settings
    DocItem<int>   settings_n_elements_limb{ 25, this, &Domain<int>::pos};
    DocItem<int> settings_n_elements_string{ 25, this, &Domain<int>::pos};
    DocItem<int>      settings_n_draw_steps{ 50, this, &Domain<int>::pos};
    DocItem<double>    settings_step_factor{0.5, this, &Domain<double>::pos};

    void load(const std::string& path)
    {
        // Todo: Handle file not existing or inability to parse
        std::ifstream file(path);
        if(!file)
        {
            throw std::runtime_error(strerror(errno));    // Todo: Better message with filename
        }

        jsoncons::ojson obj = jsoncons::ojson::parse(file);
        meta_version = obj["meta"]["version"].as<std::string>();
        meta_comments = obj["meta"]["comments"].as<std::string>();
        profile_curvature = obj["profile"]["curvature"].as<Series>();
        profile_offset_x = obj["profile"]["offset_x"].as<double>();
        profile_offset_y = obj["profile"]["offset_y"].as<double>();
        profile_angle = obj["profile"]["angle"].as<double>();
        sections_width = obj["sections"]["width"].as<Series>();
        sections_height = obj["sections"]["height"].as<Series>();
        sections_rho = obj["sections"]["rho"].as<double>();
        sections_E = obj["sections"]["E"].as<double>();
        string_strand_stiffness = obj["string"]["strand_stiffness"].as<double>();
        string_strand_density = obj["string"]["strand_density"].as<double>();
        string_n_strands = obj["string"]["n_strands"].as<double>();
        operation_brace_height = obj["operation"]["brace_height"].as<double>();
        operation_draw_length = obj["operation"]["draw_length"].as<double>();
        operation_mass_arrow = obj["operation"]["mass_arrow"].as<double>();
        mass_string_center = obj["masses"]["string_center"].as<double>();
        mass_string_tip = obj["masses"]["string_tip"].as<double>();
        mass_limb_tip = obj["masses"]["limb_tip"].as<double>();
        settings_n_elements_limb = obj["settings"]["n_elements_limb"].as<int>();
        settings_n_elements_string = obj["settings"]["n_elements_string"].as<int>();
        settings_n_draw_steps = obj["settings"]["n_draw_steps"].as<int>();
        settings_step_factor = obj["settings"]["step_factor"].as<double>();
    }

    void save(const std::string& path)
    {
        jsoncons::ojson obj;
        obj["meta"]["version"] = std::string(meta_version);
        obj["meta"]["comments"] = std::string(meta_comments);
        obj["profile"]["curvature"] = Series(profile_curvature);
        obj["profile"]["offset_x"] = double(profile_offset_x);
        obj["profile"]["offset_y"] = double(profile_offset_y);
        obj["profile"]["angle"] = double(profile_angle);
        obj["sections"]["width"] = Series(sections_width);
        obj["sections"]["height"] = Series(sections_height);
        obj["sections"]["rho"] = double(sections_rho);
        obj["sections"]["E"] = double(sections_E);
        obj["string"]["strand_stiffness"] = double(string_strand_stiffness);
        obj["string"]["strand_density"] = double(string_strand_density);
        obj["string"]["n_strands"] = double(string_n_strands);
        obj["operation"]["brace_height"] = double(operation_brace_height);
        obj["operation"]["draw_length"] = double(operation_draw_length);
        obj["operation"]["mass_arrow"] = double(operation_mass_arrow);
        obj["masses"]["string_center"] = double(mass_string_center);
        obj["masses"]["string_tip"] = double(mass_string_tip);
        obj["masses"]["limb_tip"] = double(mass_limb_tip);
        obj["settings"]["n_elements_limb"] = int(settings_n_elements_limb);
        obj["settings"]["n_elements_string"] = int(settings_n_elements_string);
        obj["settings"]["n_draw_steps"] = int(settings_n_draw_steps);
        obj["settings"]["step_factor"] = double(settings_step_factor);

        // Todo: Handle file not existing
        std::ofstream file(path);
        if(!file)
        {
            throw std::runtime_error(strerror(errno));    // Todo: Better message with filename
        }
        file << jsoncons::pretty_print(obj);
    }
};
