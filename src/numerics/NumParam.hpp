#pragma once
#include "Domain.hpp"

#include <cereal/cereal.hpp>
#include <stdexcept>

template<typename T, Domain D>
class NumParam
{
public:
    NumParam(double rhs): value(rhs)
    {
        check_domain<D>(rhs);
    }

    operator double() const
    {
        return value;
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(value));
    }

private:
    double value;
};
