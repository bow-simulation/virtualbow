#pragma once
#include "gui/Application.hpp"
#include "model/document/Document.hpp"
#include "model/Compatibility.hpp"
#include "numerics/Series.hpp"

#include <QtCore>

/*
struct Layer
{
    std::string name;
    Series height;
    double rho;
    double E;
};
*/

struct Meta: public DocumentNode
{
    DocumentItem<std::string>  version{*this, validators::any<std::string>, Application::version};
    DocumentItem<std::string> comments{*this, validators::any<std::string>, ""};
};

struct Profile: public DocumentNode
{
    DocumentItem<Series> segments{*this, validators::any<Series>, Series({0.7}, {0.0})};
    DocumentItem<double>       x0{*this, validators::any<double>, 0.0};
    DocumentItem<double>       y0{*this, validators::any<double>, 0.0};
    DocumentItem<double>     phi0{*this, validators::any<double>, 0.0};
};

struct Sections: public DocumentNode
{
    DocumentItem<Series>  width{*this, validators::any<Series>, Series({0.0, 0.5, 1.0}, {0.04, 0.035, 0.01})};
    DocumentItem<Series> height{*this, validators::any<Series>, Series({0.0, 1.0}, {0.0128, 0.008})};
    DocumentItem<double>    rho{*this, validators::pos<double>, 600.0};
    DocumentItem<double>      E{*this, validators::pos<double>, 15e9};
};

struct String: public DocumentNode
{
    DocumentItem<double> strand_stiffness{*this, validators::pos<double>, 3500.0};
    DocumentItem<double>   strand_density{*this, validators::pos<double>, 0.0005};
    DocumentItem<double>        n_strands{*this, validators::pos<double>, 12.0};
};

struct Masses: public DocumentNode
{
    DocumentItem<double> string_center{*this, validators::non_neg<double>, 0.005};
    DocumentItem<double>    string_tip{*this, validators::non_neg<double>, 0.005};
    DocumentItem<double>      limb_tip{*this, validators::non_neg<double>, 0.005};
};

struct Operation: public DocumentNode
{
    DocumentItem<double> brace_height{*this, validators::any<double>, 0.2};
    DocumentItem<double>  draw_length{*this, validators::any<double>, 0.7};
    DocumentItem<double>   mass_arrow{*this, validators::pos<double>, 0.025};
};

struct Settings: public DocumentNode
{
    DocumentItem<int>     n_elements_limb{*this, validators::pos<int>, 40};
    DocumentItem<int>   n_elements_string{*this, validators::pos<int>, 45};
    DocumentItem<int>        n_draw_steps{*this, validators::pos<int>, 150};
    DocumentItem<double> time_span_factor{*this, validators::pos<double>, 1.5};
    DocumentItem<double> time_step_factor{*this, validators::pos<double>, 0.5};
    DocumentItem<double>    sampling_rate{*this, validators::pos<double>, 1e4};
};

struct InputData: public DocumentNode
{
    Meta meta;
    Profile profile;
    Sections sections;
    String string;
    Masses masses;
    Operation operation;
    Settings settings;

    void load(const QString& path);
    void save(const QString& path);
};
