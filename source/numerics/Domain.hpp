#pragma once
#include <stdexcept>

enum class Domain
{
    All,
    Pos,
    Neg,
};

// Todo: Use constexpr and switch somehow?
template<Domain domain>
class DomainInfo
{
public:
    static double default_value();
    static bool contains(double value);
};

