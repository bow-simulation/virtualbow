#pragma once
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
