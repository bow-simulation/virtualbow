#pragma once
#include <cereal/cereal.hpp>
#include <stdexcept>

enum class Domain
{
    All,
    Pos,       // >  0
    Neg,       // <  0
    NonPos,    // <= 0
    NonNeg,    // >= 0
};

template<Domain>
void check_domain(double val);

template<>
void check_domain<Domain::All>(double val)
{

}

template<>
void check_domain<Domain::Pos>(double val)
{
    if(val <= 0)
        throw std::runtime_error("Value must be positive");
}

template<>
void check_domain<Domain::Neg>(double val)
{
    if(val >= 0)
        throw std::runtime_error("Value must be negative");
}

template<>
void check_domain<Domain::NonPos>(double val)
{
    if(val > 0)
        throw std::runtime_error("Value must not be positive");
}

template<>
void check_domain<Domain::NonNeg>(double val)
{
    if(val < 0)
        throw std::runtime_error("Value must not be negative");
}


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
