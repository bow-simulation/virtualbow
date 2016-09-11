#include "Domain.hpp"

// Default value

template<>
double DomainInfo<Domain::All>::default_value()
{
    return 0.0;
}


template<>
double DomainInfo<Domain::Pos>::default_value()
{
    return 1.0;
}

template<>
double DomainInfo<Domain::Neg>::default_value()
{
    return -1.0;
}

// Contains

template<>
bool DomainInfo<Domain::All>::contains(double value)
{
    return true;
}

template<>
bool DomainInfo<Domain::Pos>::contains(double value)
{
    return value > 0.0;
}

template<>
bool DomainInfo<Domain::Neg>::contains(double value)
{
    return value < 0.0;
}
