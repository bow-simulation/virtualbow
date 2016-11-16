#pragma once
#include "Document.hpp"
#include "../numerics/Series.hpp"
#include <QtCore>
#include <jsoncons/json.hpp>

struct InputData: public Document
{
    // Meta
    DocItem<std::string>  meta_version{this};
    DocItem<std::string> meta_comments{this};

    // Profile
    DocItem<Series> profile_segments{this};
    DocItem<double> profile_x0{this};
    DocItem<double> profile_y0{this};
    DocItem<double>    profile_phi0{this};

    // Sections
    DocItem<Series>  sections_width{this};
    DocItem<Series> sections_height{this};
    DocItem<double>   sections_rho {this, &Domain<double>::pos};
    DocItem<double>      sections_E{this, &Domain<double>::pos};

    // String
    DocItem<double> string_strand_stiffness{this, &Domain<double>::pos};
    DocItem<double>   string_strand_density{this, &Domain<double>::pos};
    DocItem<double>        string_n_strands{this, &Domain<double>::pos};

    // Operation
    DocItem<double> operation_brace_height{this};
    DocItem<double>  operation_draw_length{this, &Domain<double>::pos};
    DocItem<double>   operation_mass_arrow{this, &Domain<double>::pos};

    // Additional masses
    DocItem<double> mass_string_center{this, &Domain<double>::non_neg};
    DocItem<double>    mass_string_tip{this, &Domain<double>::non_neg};
    DocItem<double>      mass_limb_tip{this, &Domain<double>::non_neg};

    // Settings
    DocItem<int>   settings_n_elements_limb{this, &Domain<int>::pos};
    DocItem<int> settings_n_elements_string{this, &Domain<int>::pos};
    DocItem<int>      settings_n_draw_steps{this, &Domain<int>::pos};
    DocItem<double>    settings_time_span_factor{this, &Domain<double>::pos};
    DocItem<double>    settings_time_step_factor{this, &Domain<double>::pos};
    DocItem<double>    settings_sampling_time{this, &Domain<double>::pos};

    InputData(const QString& path)
    {
        load(path);
    }

    void load(const QString& path)
    {
        QFile file(path);
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
            throw std::runtime_error("Could not open file");    // Todo: Better message with filename
        }

        std::string str = QTextStream(&file).readAll().toStdString();
        jsoncons::ojson obj = jsoncons::ojson::parse_string(str);

        meta_version = obj["meta"]["version"].as<std::string>();
        meta_comments = obj["meta"]["comments"].as<std::string>();
        profile_segments = obj["profile"]["segments"].as<Series>();
        profile_x0 = obj["profile"]["offset_x"].as<double>();
        profile_y0 = obj["profile"]["offset_y"].as<double>();
        profile_phi0 = obj["profile"]["angle"].as<double>();
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
        settings_time_span_factor = obj["settings"]["time_span_factor"].as<double>();
        settings_time_step_factor = obj["settings"]["time_step_factor"].as<double>();
        settings_sampling_time = obj["settings"]["sampling_time"].as<double>();

        this->setModified(false);
    }

    void save(const QString& path)
    {
        jsoncons::ojson obj;
        obj["meta"]["version"] = std::string(meta_version);
        obj["meta"]["comments"] = std::string(meta_comments);
        obj["profile"]["segments"] = Series(profile_segments);
        obj["profile"]["offset_x"] = double(profile_x0);
        obj["profile"]["offset_y"] = double(profile_y0);
        obj["profile"]["angle"] = double(profile_phi0);
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
        obj["settings"]["time_span_factor"] = double(settings_time_span_factor);
        obj["settings"]["time_step_factor"] = double(settings_time_step_factor);
        obj["settings"]["sampling_time"] = double(settings_sampling_time);

        QFile file(path);
        if(!file.open(QFile::WriteOnly | QFile::Text))
        {
            throw std::runtime_error("Could not open file");    // Todo: Better message with filename
        }

        std::ostringstream oss;
        oss << jsoncons::pretty_print(obj);
        QTextStream(&file) << QString::fromStdString(oss.str());

        this->setModified(false);
    }
};
