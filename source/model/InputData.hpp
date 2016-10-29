#pragma once
#include "../numerics/DataSeries.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

struct Limb
{
    struct Layer
    {
        std::string name = "";
        DataSeries height = {{0.0, 1.0}, {0.01, 0.008}};
        double rho = 500.0;
        double E = 40.0e9;

        template<class Archive>
        void serialize(Archive & archive)
        {
            archive(CEREAL_NVP(name),
                    CEREAL_NVP(height),
                    CEREAL_NVP(rho),
                    CEREAL_NVP(E));
        }
    };

    DataSeries width = {{0.0, 1.0}, {0.05, 0.01}};
    std::vector<Layer> layers = {Layer()};
    DataSeries curvature = {{0.3, 0.4}, {-1.0, 1.0}};
    double offset_x = 0.0;
    double offset_y = 0.0;
    double angle = 0.0;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(width),
                CEREAL_NVP(layers),
                CEREAL_NVP(curvature),
                CEREAL_NVP(offset_x),
                CEREAL_NVP(offset_y),
                CEREAL_NVP(angle));
    }
};

struct String
{
    double strand_stiffness = 3100.0;
    double strand_density = 0.05;
    double n_strands = 10.0;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(strand_stiffness),
                CEREAL_NVP(strand_density),
                CEREAL_NVP(n_strands));
    }
};

struct Operation
{
    double brace_height = 0.2;
    double draw_length = 0.7;
    double arrow_mass = 0.02;

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
    unsigned n_elements_limb = 25;
    unsigned n_elements_string = 25;
    double step_factor = 0.5;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(n_elements_limb),
                CEREAL_NVP(n_elements_string),
                CEREAL_NVP(step_factor));
    }
};

struct MetaData
{
    std::string comments = "";
    std::string version = "";    // Todo: Get default value from somewhere?

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
    MetaData meta;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(limb),
                CEREAL_NVP(string),
                CEREAL_NVP(operation),
                CEREAL_NVP(settings),
                CEREAL_NVP(meta));
    }

    void load(const std::string& current_file)
    {
        // Todo: Handle file not existing or inability to parse
        std::ifstream file(current_file);
        if(!file)
        {
            throw std::runtime_error(strerror(errno));    // Todo: Better message with filename
        }

        cereal::JSONInputArchive archive(file);
        serialize(archive);
    }

    void save(const std::string& current_file)
    {
        // Todo: Handle file not existing
        std::ofstream file(current_file);
        if(!file)
        {
            throw std::runtime_error(strerror(errno));    // Todo: Better message with filename
        }

        cereal::JSONOutputArchive archive(file);
        serialize(archive);
    }
};
