#pragma once
#include "Document.hpp"
#include "../numerics/DataSeries.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

struct InputData: public Document
{
    // Profile
    DocItem<DataSeries> profile_curvature{this, {{0.3, 0.4}, {-1.0, 1.0}}};
    DocItem<double> profile_offset_x{this, 0.0};
    DocItem<double> profile_offset_y{this, 0.0};
    DocItem<double> profile_angle{this, 0.0};

    // Sections
    DocItem<DataSeries> sections_width{this, {{0.0, 1.0}, {0.01, 0.008}}};
    DocItem<DataSeries> sections_height{this, {{0.0, 1.0}, {0.01, 0.008}}};
    DocItem<double> sections_rho{this, 500.0};
    DocItem<double> sections_E{this, 40.0e9};

    // String
    DocItem<double> string_strand_stiffness{this, 3100.0};
    DocItem<double> string_strand_density{this, 0.05};
    DocItem<double> string_n_strands{this, 10.0};

    // Operation
    DocItem<double> operation_brace_height{this, 0.2};
    DocItem<double> operation_draw_length{this, 0.7};
    DocItem<double> operation_mass_arrow{this, 0.02};

    // Additional masses
    DocItem<double> mass_string_center{this, 0.0};
    DocItem<double> mass_string_tip{this, 0.0};
    DocItem<double> mass_limb_tip{this, 0.0};

    // Settings
    DocItem<unsigned> settings_n_elements_limb{this, 25};
    DocItem<unsigned> settings_n_elements_string{this, 25};
    DocItem<unsigned> settings_n_draw_steps{this, 50};
    DocItem<double> settings_step_factor{this, 0.5};

    // Meta
    DocItem<std::string> meta_comments{this, ""};
    DocItem<std::string> meta_version{this, ""};    // Todo: Get default value from somewhere? Perhaps set in GUI appliation.

    template<class Archive>
    void serialize(Archive & archive)
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
