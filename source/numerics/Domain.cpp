#include "Domain.hpp"

// Default value

template<>
double Domain<DomainTag::All>::default_value()
{
    return 0.0;
}


template<>
double Domain<DomainTag::Pos>::default_value()
{
    return 1.0;
}

template<>
double Domain<DomainTag::Neg>::default_value()
{
    return -1.0;
}

template<>
double Domain<DomainTag::NonPos>::default_value()
{
    return 0.0;
}

template<>
double Domain<DomainTag::NonNeg>::default_value()
{
    return 0.0;
}

// Contains

template<>
bool Domain<DomainTag::All>::contains(double value)
{
    return true;
}

template<>
bool Domain<DomainTag::Pos>::contains(double value)
{
    return value > 0.0;
}

template<>
bool Domain<DomainTag::Neg>::contains(double value)
{
    return value < 0.0;
}

template<>
bool Domain<DomainTag::NonPos>::contains(double value)
{
    return value <= 0.0;
}

template<>
bool Domain<DomainTag::NonNeg>::contains(double value)
{
    return value >= 0.0;
}
