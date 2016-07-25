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
    double offset;
    double angle;

    // Default parameters
    Parameters()
        : curvature{{{0.2, 0.2, 0.2}}, {{1.0, 2.0, -5.0}}},
          offset(0.0),
          angle(0.0)
    {

    }

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(curvature),
                CEREAL_NVP(offset),
                CEREAL_NVP(angle));
    }
};
