#pragma once
#include "Document.hpp"
#include "../numerics/DataSeries.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

struct InputData: public Document
{
    // Profile
    DocItem<DataSeries> profile_curvature{{{0.3, 0.4}, {-1.0, 1.0}}, this};
    DocItem<double> profile_offset_x{0.0, this};
    DocItem<double> profile_offset_y{0.0, this};
    DocItem<double>    profile_angle{0.0, this};

    // Sections
    DocItem<DataSeries> sections_width{{{0.0, 1.0}, {0.01, 0.008}}, this};
    DocItem<DataSeries> sections_height{{{0.0, 1.0}, {0.01, 0.008}}, this};
    DocItem<double> sections_rho {500.0, this, &Domain<double>::pos};
    DocItem<double>   sections_E{40.0e9, this, &Domain<double>::pos};

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

    // Meta
    DocItem<std::string> meta_comments{"", this};
    DocItem<std::string>  meta_version{"", this};    // Todo: Get default value from somewhere? Perhaps set in GUI appliation.

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(profile_curvature),
                CEREAL_NVP(profile_offset_x),
                CEREAL_NVP(profile_offset_y),
                CEREAL_NVP(profile_angle),
                CEREAL_NVP(sections_width),
                CEREAL_NVP(sections_height),
                CEREAL_NVP(sections_rho),
                CEREAL_NVP(sections_E),
                CEREAL_NVP(string_strand_stiffness),
                CEREAL_NVP(string_strand_density),
                CEREAL_NVP(string_n_strands),
                CEREAL_NVP(operation_brace_height),
                CEREAL_NVP(operation_draw_length),
                CEREAL_NVP(operation_mass_arrow),
                CEREAL_NVP(mass_string_center),
                CEREAL_NVP(mass_string_tip),
                CEREAL_NVP(mass_limb_tip),
                CEREAL_NVP(settings_n_elements_limb),
                CEREAL_NVP(settings_n_elements_string),
                CEREAL_NVP(settings_n_draw_steps),
                CEREAL_NVP(settings_step_factor),
                CEREAL_NVP(meta_comments),
                CEREAL_NVP(meta_version));
    }

    void load(const std::string& path)
    {
        // Todo: Handle file not existing or inability to parse
        std::ifstream file(path);
        if(!file)
        {
            throw std::runtime_error(strerror(errno));    // Todo: Better message with filename
        }

        cereal::JSONInputArchive archive(file);
        serialize(archive);
    }

    void save(const std::string& path)
    {
        // Todo: Handle file not existing
        std::ofstream file(path);
        if(!file)
        {
            throw std::runtime_error(strerror(errno));    // Todo: Better message with filename
        }

        cereal::JSONOutputArchive archive(file);
        serialize(archive);
    }
};
