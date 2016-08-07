#pragma once
#include "../numerics/DataSeries.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

struct BowParameters
{
    DataSeries width = {{0.0, 1.0}, {0.05, 0.01}};
    DataSeries height = {{0.0, 1.0}, {0.01, 0.01}};
    DataSeries curvature = {{0.7}, {0.0}};
    double angle = 0.0;
    double offset = 0.0;

    void load(const std::string& path)
    {
        std::ifstream file(path);
        cereal::JSONInputArchive archive(file);
        serialize(archive);
    }

    void save(const std::string& path)
    {
        std::ofstream file(path);
        cereal::JSONOutputArchive archive(file);
        serialize(archive);
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(width),
                CEREAL_NVP(height),
                CEREAL_NVP(curvature),
                CEREAL_NVP(angle),
                CEREAL_NVP(offset));
    }

};
