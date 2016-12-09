#pragma once

struct Dof
{
    bool active;
    size_t index;

    bool operator!=(const Dof& rhs) const
    {
        return (rhs.active != active) || (rhs.index != index);
    }
};

struct Node
{
    Dof x;
    Dof y;
    Dof phi;
};
