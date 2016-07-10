#pragma once
#include <cereal/cereal.hpp>

#include <vector>
#include <algorithm>
#include <cassert>

using std::size_t;

class StepFunction
{
public:
    struct Parameters;
    StepFunction(Parameters p);

    double operator()(double arg) const;
    double arg_min() const;
    double arg_max() const;

private:
    size_t lower_index(double arg) const;

    std::vector<double> intervals;
    std::vector<double> values;
};

struct StepFunction::Parameters
{
    std::vector<double> widths;
    std::vector<double> values;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(widths),
                CEREAL_NVP(values));
    }

    bool is_valid()
    {
        if(widths.size() != values.size())
            return false;

        // Todo: Shorter way to check this?
        for(double w: widths)
        {
            if(w <= 0)
                return false;
        }

        return true;
    }
};
