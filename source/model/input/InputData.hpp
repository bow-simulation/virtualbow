#pragma once
#include "model/document/Document.hpp"
#include "Meta.hpp"
#include "Profile.hpp"
#include "Material.hpp"
#include "String.hpp"
#include "Masses.hpp"
#include "Operation.hpp"
#include "Settings.hpp"
#include <json.hpp>
#include <fstream>

#include "numerics/CubicSpline.hpp"
#include "model/LimbProperties.hpp"

using nlohmann::json;

struct InputData: public DocumentNode
{
    Meta                   meta{*this};
    Profile             profile{*this};
    DocumentItem<Series>  width{*this, Series({0.0, 0.5, 1.0}, {0.04, 0.035, 0.01})};
    DocumentItem<Series> height{*this, Series({0.0, 1.0}, {0.0128, 0.008})};
    Material           material{*this};
    String               string{*this};
    Masses               masses{*this};
    Operation         operation{*this};
    Settings           settings{*this};

    InputData()
    {
        // Set up constraints for input validation here
        // Todo: Lots of code duplication below

        create_constraint(material.rho, "Density must be positive",         [](double x){ return x > 0; });
        create_constraint(material.E,   "Elastic modulus must be positive", [](double x){ return x > 0; });

        create_constraint(string.strand_stiffness, "Strand stiffness must be positive",  [](double x){ return x > 0; });
        create_constraint(string.strand_density,   "Strand density must be positive",    [](double x){ return x > 0; });
        create_constraint(string.n_strands,        "Number of strands must be positive", [](double x){ return x > 0; });

        create_constraint(masses.string_center, "String center mass must be positive", [](double x){ return x > 0; });
        create_constraint(masses.string_tip,    "String tip mass must be positive",    [](double x){ return x > 0; });
        create_constraint(masses.limb_tip,      "Limb tip mass must be positive",      [](double x){ return x > 0; });

        create_constraint(operation.draw_length, operation.brace_height, "Draw length must be larger than brace height", [](double x, double y){ return x > y; });
        create_constraint(operation.mass_arrow, "Arrow mass must be positive", [](double x){ return x > 0; });

        create_constraint(settings.n_elements_limb,   "Number of limb elements must be positive",   [](int x){ return x > 0; });
        create_constraint(settings.n_elements_string, "Number of string elements must be positive", [](int x){ return x > 0; });
        create_constraint(settings.n_draw_steps,      "Number of draw steps must be positive",      [](int x){ return x > 0; });
        create_constraint(settings.time_span_factor,  "Time span factor must be positive",          [](double x){ return x > 0; });
        create_constraint(settings.time_step_factor,  "Time step factor must be between 0 and 1",   [](double x){ return x > 0.0 && x < 1.0; });
        create_constraint(settings.sampling_rate,     "Sampling rate must be positive",             [](double x){ return x > 0; });

        create_constraint(width, "Supplied widths must be positive", [](const Series& width)
        {
            for(double w: width.vals()) {
                if(w <= 0)
                    return false;
            }

            return true;
        });

        create_constraint(height, "Supplied heights must be positive", [](const Series& height) {
            for(double h: height.vals()) {
                if(h <= 0)
                    return false;
            }

            return true;
        });

        create_constraint(width, "Minimum width must be positive", [](const Series& width)
        {
            try
            {
                Series output = CubicSpline::sample(width, 150);    // Magic number
                for(double w: output.vals()) {
                    if(w <= 0)
                        return false;
                }
            }
            catch(...)
            {
                // Carry on, don't treat other problems here
            }

            return true;
        });

        create_constraint(height, "Minimum height must be positive", [](const Series& width)
        {
            try
            {
                Series output = CubicSpline::sample(width, 150);    // Magic number
                for(double h: output.vals()) {
                    if(h <= 0)
                        return false;
                }
            }
            catch(...)
            {
                // Carry on, don't treat other problems here
            }

            return true;
        });

        create_constraint(operation.brace_height, "Brace height too low for current limb profile",
            [&](double brace_height)
        {
            try
            {
                LimbProperties limb(*this);
                for(size_t i = 0; i < limb.y.size(); ++i) {
                    if(limb.y[i] <= -brace_height)
                        return false;
                }
            }
            catch(...)
            {
                // Carry on, don't treat other problems here
            }

            return true;
        });
    }

    void load(const json& obj)
    {
        meta.load(obj["meta"]);
        profile.load(obj["profile"]);
        width  = (Series) obj["width"];    // Todo: Unify interface
        height = (Series) obj["height"];
        material.load(obj["sections"]);
        string.load(obj["string"]);
        masses.load(obj["masses"]);
        operation.load(obj["operation"]);
        settings.load(obj["settings"]);
    }

    void save(json& obj) const
    {
        meta.save(obj["meta"]);
        profile.save(obj["profile"]);
        obj["width"]  = (Series) width;    // Todo: Unify interface
        obj["height"] = (Series) height;
        material.save(obj["sections"]);
        string.save(obj["string"]);
        masses.save(obj["masses"]);
        operation.save(obj["operation"]);
        settings.save(obj["settings"]);
    }

    void load(const std::string& path)
    {
        std::ifstream stream(path);

        json obj;
        obj << stream;

        load(obj);
    }

    void save(const std::string& path)
    {
        json obj;
        save(obj);

        std::ofstream stream(path);
        stream << std::setw(4) << obj << std::endl;
    }

private:
    template<typename T, typename F>
    void create_constraint(DocumentItem<T>& item, const std::string& message, const F& validator)
    {
        QObject::connect(this, &DocumentNode::value_changed, [&item, message, validator]()
        {
            if(validator(item))
            {
                item.remove_error(message);
            }
            else
            {
                item.add_error(message);
            }
        });
    }

    template<typename T1, typename T2, typename F>
    void create_constraint(DocumentItem<T1>& item1, DocumentItem<T2>& item2, const std::string& message, const F& validator)
    {
        QObject::connect(this, &DocumentNode::value_changed, [&item1, &item2, message, validator]()
        {
            if(validator(item1, item2))
            {
                item1.remove_error(message);
                item2.remove_error(message);
            }
            else
            {
                item1.add_error(message);
                item2.add_error(message);
            }
        });

        // Todo: Code duplication
        QObject::connect(this, &DocumentNode::value_changed, [&item1, &item2, message, validator]()
        {
            if(validator(item1, item2))
            {
                item1.remove_error(message);
                item2.remove_error(message);
            }
            else
            {
                item1.add_error(message);
                item2.add_error(message);
            }
        });
    }
};
