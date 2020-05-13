#pragma once
#include <cstddef>

enum class DofType
{
    Active,
    Fixed
};

struct Dof
{
    DofType type;
    size_t index;
};

struct Node
{
    Dof x;
    Dof y;
    Dof phi;
};
