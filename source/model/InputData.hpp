#pragma once
#include "gui/Application.hpp"
#include "model/document/Document.hpp"
#include "model/Compatibility.hpp"
#include "numerics/Series.hpp"

#include <QtCore>

struct Layer
{
    std::string name;
    Series height;
    double rho;
    double E;
};

struct InputData: public DocumentNode
{
    // Meta
    DocumentItem<std::string>  meta_version{*this, validators::any<std::string>, Application::version};
    DocumentItem<std::string> meta_comments{*this, validators::any<std::string>, ""};

    // Profile
    DocumentItem<Series> profile_segments{*this, validators::any<Series>, Series({0.7}, {0.0})};
    DocumentItem<double>       profile_x0{*this, validators::any<double>, 0.0};
    DocumentItem<double>       profile_y0{*this, validators::any<double>, 0.0};
    DocumentItem<double>     profile_phi0{*this, validators::any<double>, 0.0};

    // Sections
    DocumentItem<Series>  sections_width{*this, validators::any<Series>, Series({0.0, 0.5, 1.0}, {0.04, 0.035, 0.01})};
    DocumentItem<Series> sections_height{*this, validators::any<Series>, Series({0.0, 1.0}, {0.0128, 0.008})};
    DocumentItem<double>    sections_rho{*this, validators::pos<double>, 600.0};
    DocumentItem<double>      sections_E{*this, validators::pos<double>, 15e9};

    // String
    DocumentItem<double> string_strand_stiffness{*this, validators::pos<double>, 3500.0};
    DocumentItem<double>   string_strand_density{*this, validators::pos<double>, 0.0005};
    DocumentItem<double>        string_n_strands{*this, validators::pos<double>, 12.0};

    // Additional masses
    DocumentItem<double> mass_string_center{*this, validators::non_neg<double>, 0.005};
    DocumentItem<double>    mass_string_tip{*this, validators::non_neg<double>, 0.005};
    DocumentItem<double>      mass_limb_tip{*this, validators::non_neg<double>, 0.005};

    // Operation
    DocumentItem<double> operation_brace_height{*this, validators::any<double>, 0.2};
    DocumentItem<double>  operation_draw_length{*this, validators::any<double>, 0.7};
    DocumentItem<double>   operation_mass_arrow{*this, validators::pos<double>, 0.025};

    // Settings
    DocumentItem<int>     settings_n_elements_limb{*this, validators::pos<int>, 40};
    DocumentItem<int>   settings_n_elements_string{*this, validators::pos<int>, 45};
    DocumentItem<int>        settings_n_draw_steps{*this, validators::pos<int>, 150};
    DocumentItem<double> settings_time_span_factor{*this, validators::pos<double>, 1.5};
    DocumentItem<double> settings_time_step_factor{*this, validators::pos<double>, 0.5};
    DocumentItem<double>    settings_sampling_rate{*this, validators::pos<double>, 1e4};

    // Todo: Use std library
    InputData();
    void load(const QString& path);
    void save(const QString& path);
};
