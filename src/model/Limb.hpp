#pragma once
#include "../numerics/StepFunction.hpp"

#include <vector>

struct Limb
{
    struct Parameters;
    Limb(Parameters p);

    std::vector<double> s;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> phi;
};

struct Limb::Parameters
{
    StepFunction::Parameters curvature;
    double riser_width;
    double riser_angle;
    size_t n_elements;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(curvature),
                CEREAL_NVP(riser_width),
                CEREAL_NVP(riser_angle),
                CEREAL_NVP(n_elements));
    }

    bool is_valid()
    {
        return true;
    }
};
