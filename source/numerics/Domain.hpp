#pragma once
#include <stdexcept>

enum class DomainTag
{
    All,
    Pos,
    Neg,
    NonPos,
    NonNeg,
};

// Todo: Use constexpr and switch somehow?
template<DomainTag domain>
class Domain
{
public:
    static double default_value();
    static bool contains(double value);
};

