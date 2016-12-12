#pragma once
#include "Document.hpp"
#include "../numerics/Series.hpp"
#include <QtCore>

struct InputData: public Document
{
    // Meta
    DocItem<std::string>  meta_version{this};
    DocItem<std::string> meta_comments{this};

    // Profile
    DocItem<Series> profile_segments{this};
    DocItem<double>       profile_x0{this};
    DocItem<double>       profile_y0{this};
    DocItem<double>     profile_phi0{this};

    // Sections
    DocItem<Series>  sections_width{this};
    DocItem<Series> sections_height{this};
    DocItem<double>   sections_rho {this, &Domain<double>::pos};
    DocItem<double>      sections_E{this, &Domain<double>::pos};

    // String
    DocItem<double> string_strand_stiffness{this, &Domain<double>::pos};
    DocItem<double>   string_strand_density{this, &Domain<double>::pos};
    DocItem<double>        string_n_strands{this, &Domain<double>::pos};

    // Additional masses
    DocItem<double> mass_string_center{this, &Domain<double>::non_neg};
    DocItem<double>    mass_string_tip{this, &Domain<double>::non_neg};
    DocItem<double>      mass_limb_tip{this, &Domain<double>::non_neg};

    // Operation
    DocItem<double> operation_brace_height{this};
    DocItem<double>  operation_draw_length{this, &Domain<double>::pos};
    DocItem<double>   operation_mass_arrow{this, &Domain<double>::pos};

    // Settings
    DocItem<int>     settings_n_elements_limb{this, &Domain<int>::pos};
    DocItem<int>   settings_n_elements_string{this, &Domain<int>::pos};
    DocItem<int>        settings_n_draw_steps{this, &Domain<int>::pos};
    DocItem<double> settings_time_span_factor{this, &Domain<double>::pos};
    DocItem<double> settings_time_step_factor{this, &Domain<double>::pos};
    DocItem<double>    settings_sampling_time{this, &Domain<double>::pos};

    InputData(const QString& path);
    void load(const QString& path);
    void save(const QString& path);
};
