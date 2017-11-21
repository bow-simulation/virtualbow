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
        // Todo: Lots of code duplication below

        create_constraint(width, "Supplied widths must be positive", [](const Series& width) {
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

        create_constraint(width, "Minimum width must be positive", [](const Series& width) {
            Series output = CubicSpline::sample(width, 150);    // Magic number
            for(double w: output.vals()) {
                if(w <= 0)
                    return false;
            }

            return true;
        });

        create_constraint(height, "Minimum height must be positive", [](const Series& width) {
            Series output = CubicSpline::sample(width, 150);    // Magic number
            for(double h: output.vals()) {
                if(h <= 0)
                    return false;
            }

            return true;
        });

        create_constraint(operation.brace_height, profile, "Brace height too low for current limb profile", [&](double brace_height) {
            LimbProperties limb(*this);
            for(size_t i = 0; i < limb.y.size(); ++i) {
                if(limb.y[i] <= -brace_height)
                    return false;
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
};
