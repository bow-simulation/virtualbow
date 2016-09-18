#include "Domain.hpp"
#include <limits>

// All

template<>
double Domain<DomainTag::All>::min()
{
    return std::numeric_limits<double>::lowest();
}

template<>
double Domain<DomainTag::All>::max()
{
    return std::numeric_limits<double>::max();
}

// Pos

template<>
double Domain<DomainTag::Pos>::min()
{
    return std::numeric_limits<double>::min();
}

template<>
double Domain<DomainTag::Pos>::max()
{
    return std::numeric_limits<double>::max();
}

// Neg

template<>
double Domain<DomainTag::Neg>::min()
{
    return std::numeric_limits<double>::lowest();
}

template<>
double Domain<DomainTag::Neg>::max()
{
    return -std::numeric_limits<double>::min();
}

// NonPos

template<>
double Domain<DomainTag::NonPos>::min()
{
    return std::numeric_limits<double>::lowest();
}

template<>
double Domain<DomainTag::NonPos>::max()
{
    return 0.0;
}

// NonNeg

template<>
double Domain<DomainTag::NonNeg>::min()
{
    return 0.0;
}

template<>
double Domain<DomainTag::NonNeg>::max()
{
    return std::numeric_limits<double>::max();
}

