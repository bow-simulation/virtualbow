#pragma once
#include "TDMatrix.hpp"

#include <cereal/cereal.hpp>

class SplineFunction
{
public:
    struct Parameters;
    SplineFunction(Parameters p);
    double operator()(double arg) const;

    // Todo: Factor this out and make it work for all function objects
    void sample(std::vector<double>& args, std::vector<double>& values, size_t n_sample) const;

private:
    size_t interval_index(double arg) const;

    std::vector<double> t;
    std::vector<double> x;

    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> c;
};

struct SplineFunction::Parameters
{
    std::vector<double> args;
    std::vector<double> values;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(args),
                CEREAL_NVP(values));
    }

    bool is_valid()
    {
        if(args.size() != values.size() || args.size() < 2)
            return false;

        for(size_t i = 0; i < args.size() - 1; ++i)
        {
            if(args[i] >= args[i+1])
                return false;
        }

        return true;
    }
};
