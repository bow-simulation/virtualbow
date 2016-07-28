#include "Domain.hpp"

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
