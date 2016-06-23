#pragma once

struct Dof
{
    enum class Type{
        Active,
        Fixed,
    };

    Type type;
    size_t index;
};

struct Node
{
    Dof x;
    Dof y;
    Dof phi;
};
