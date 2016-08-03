#pragma once
#include "../numerics/DataSeries.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

struct Limb
{
    struct Layer
    {
        std::string name;
        DataSeries height;
        double rho;
        double E;

        template<class Archive>
        void serialize(Archive & archive)
        {
            archive(CEREAL_NVP(name),
                    CEREAL_NVP(height),
                    CEREAL_NVP(rho),
                    CEREAL_NVP(E));
        }
    };

    DataSeries width;
    std::vector<Layer> layers;
    DataSeries curvature;
    double offset_x;
    double offset_y;
    double angle;
    double tip_mass;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(width),
                CEREAL_NVP(layers),
                CEREAL_NVP(curvature),
                CEREAL_NVP(offset_x),
                CEREAL_NVP(offset_y),
                CEREAL_NVP(angle),
                CEREAL_NVP(tip_mass));
    }
};

struct String
{
    double strand_stiffness;
    double strand_density;
    unsigned n_strands;

    double center_mass;
    double end_mass;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(strand_stiffness),
                CEREAL_NVP(strand_density),
                CEREAL_NVP(n_strands),
                CEREAL_NVP(center_mass),
                CEREAL_NVP(end_mass));
    }
};

struct Operation
{
    double brace_height;
    double draw_length;
    double arrow_mass;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(brace_height),
                CEREAL_NVP(draw_length),
                CEREAL_NVP(arrow_mass));
    }
};

struct Settings
{
    unsigned n_limb_elements;
    unsigned n_string_elements;
    double step_factor;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(n_limb_elements),
                CEREAL_NVP(n_string_elements),
                CEREAL_NVP(step_factor));
    }
};

struct Meta
{
    std::string comments;
    std::string version;    // Todo: Get default value from somewhere?

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(comments),
                CEREAL_NVP(version));
    }
};

struct InputData
{
    Limb limb;
    String string;
    Operation operation;
    Settings settings;
    Meta meta;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(limb),
                CEREAL_NVP(string),
                CEREAL_NVP(operation),
                CEREAL_NVP(settings),
                CEREAL_NVP(meta));
    }

    void load(const std::string& path)
    {
        // Todo: Handle file not existing or inability to parse
        std::ifstream file(path);
        cereal::JSONInputArchive archive(file);
        serialize(archive);
    }

    void save(const std::string& path)
    {
        // Todo: Handle file not existing
        std::ofstream file(path);
        cereal::JSONOutputArchive archive(file);
        serialize(archive);
    }
};
