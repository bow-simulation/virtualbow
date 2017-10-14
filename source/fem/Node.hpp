#pragma once
#include <cstddef>

struct Dof
{
    bool active;
    size_t index;
};

struct Node
{
    Dof x;
    Dof y;
    Dof phi;
};
