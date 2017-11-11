#include "InputData.hpp"
#include "gui/Application.hpp"
#include <json.hpp>

using nlohmann::json;

InputData::InputData(const QString& path)
{
    load(path);
}

void InputData::load(const QString& path)
{
    QFile file(path);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        throw std::runtime_error("Could not open file");    // Todo: Better message with filename
    }

    std::string str = QTextStream(&file).readAll().toStdString();
    json obj = json::parse(str);

    convert_to_current(obj);

    meta_version = obj["meta"]["version"].get<std::string>();
    meta_comments = obj["meta"]["comments"].get<std::string>();
    profile_segments = obj["profile"]["segments"].get<Series>();
    profile_x0 = obj["profile"]["x0"].get<double>();
    profile_y0 = obj["profile"]["y0"].get<double>();
    profile_phi0 = obj["profile"]["phi0"].get<double>();
    sections_width = obj["width"].get<Series>();
    sections_height = obj["height"].get<Series>();
    sections_rho = obj["material"]["rho"].get<double>();
    sections_E = obj["material"]["E"].get<double>();
    string_strand_stiffness = obj["string"]["strand_stiffness"].get<double>();
    string_strand_density = obj["string"]["strand_density"].get<double>();
    string_n_strands = obj["string"]["n_strands"].get<double>();
    mass_string_center = obj["masses"]["string_center"].get<double>();
    mass_string_tip = obj["masses"]["string_tip"].get<double>();
    mass_limb_tip = obj["masses"]["limb_tip"].get<double>();
    operation_brace_height = obj["operation"]["brace_height"].get<double>();
    operation_draw_length = obj["operation"]["draw_length"].get<double>();
    operation_mass_arrow = obj["operation"]["mass_arrow"].get<double>();
    settings_n_elements_limb = obj["settings"]["n_elements_limb"].get<int>();
    settings_n_elements_string = obj["settings"]["n_elements_string"].get<int>();
    settings_n_draw_steps = obj["settings"]["n_draw_steps"].get<int>();
    settings_time_span_factor = obj["settings"]["time_span_factor"].get<double>();
    settings_time_step_factor = obj["settings"]["time_step_factor"].get<double>();
    settings_sampling_rate = obj["settings"]["sampling_rate"].get<double>();
}

void InputData::save(const QString& path)
{
    json obj;
    obj["meta"]["version"] = Application::version;
    obj["meta"]["comments"] = std::string(meta_comments);
    obj["profile"]["segments"] = Series(profile_segments);
    obj["profile"]["x0"] = double(profile_x0);
    obj["profile"]["y0"] = double(profile_y0);
    obj["profile"]["phi0"] = double(profile_phi0);
    obj["width"] = Series(sections_width);
    obj["height"] = Series(sections_height);
    obj["material"]["rho"] = double(sections_rho);
    obj["material"]["E"] = double(sections_E);
    obj["string"]["strand_stiffness"] = double(string_strand_stiffness);
    obj["string"]["strand_density"] = double(string_strand_density);
    obj["string"]["n_strands"] = double(string_n_strands);
    obj["masses"]["string_center"] = double(mass_string_center);
    obj["masses"]["string_tip"] = double(mass_string_tip);
    obj["masses"]["limb_tip"] = double(mass_limb_tip);
    obj["operation"]["brace_height"] = double(operation_brace_height);
    obj["operation"]["draw_length"] = double(operation_draw_length);
    obj["operation"]["mass_arrow"] = double(operation_mass_arrow);
    obj["settings"]["n_elements_limb"] = int(settings_n_elements_limb);
    obj["settings"]["n_elements_string"] = int(settings_n_elements_string);
    obj["settings"]["n_draw_steps"] = int(settings_n_draw_steps);
    obj["settings"]["time_span_factor"] = double(settings_time_span_factor);
    obj["settings"]["time_step_factor"] = double(settings_time_step_factor);
    obj["settings"]["sampling_rate"] = double(settings_sampling_rate);

    QFile file(path);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        throw std::runtime_error("Could not open file");    // Todo: Better message with filename and exact reason
    }

    std::ostringstream oss;
    oss << std::setw(4) << obj;
    QTextStream(&file) << QString::fromStdString(oss.str());
}
