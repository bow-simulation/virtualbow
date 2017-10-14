#pragma once
#include <array>

class System;

class Dof
{
public:
    Dof(size_t i);
    Dof(double u);
    Dof();

    size_t index() const;
    bool type() const;

    bool operator!=(const Dof& rhs) const;

    bool m_active;
    union{ size_t m_i; double m_u; };
};

struct Node
{
    Dof x;
    Dof y;
    Dof phi;
};
