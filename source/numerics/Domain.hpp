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
    static bool contains(double value)
    {
        return value >= min() && value <= max();
    }

    static double min();
    static double max();
};

