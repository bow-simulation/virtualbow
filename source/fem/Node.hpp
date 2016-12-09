#pragma once
#include <array>

enum class DofType
{
    Active, Fixed
};

class System;

// Todo: Encapsulation
struct Dof
{
    System* system;
    DofType type;
    union{ size_t index; double u_fixed; };

    Dof(System& system, size_t index);
    Dof(System& system, double u);
    Dof();

    bool operator!=(const Dof& rhs) const;

    double u() const;
    double v() const;
    double a() const;
    double p() const;
    double& p_mut();
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
