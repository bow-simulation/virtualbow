#pragma once
#include <array>

class System;

enum class DofType
{
    Active, Fixed
};

class Dof
{
public:
    Dof(System& system, size_t i);
    Dof(System& system, double u);
    Dof();

    size_t index() const;
    DofType type() const;

    bool operator!=(const Dof& rhs) const;

    double u() const;
    double v() const;
    double a() const;
    double p() const;
    double& p_mut();

private:
    System* m_system;
    DofType m_type;
    union{ size_t m_i; double m_u; };
};

class Node
{
public:
    Node(std::array<Dof, 3> dofs);
    Node();

    Dof operator[](size_t i) const;

    static double distance(const Node& node_a, const Node& node_b);
    static double angle(const Node& node_a, const Node& node_b);

private:
    std::array<Dof, 3> dofs;
};
