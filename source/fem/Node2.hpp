#pragma once
#include <array>

enum class DofType
{
    Active, Fixed
};

class System2;

// Todo: Encapsulation
struct Dof2
{
    System2& system;
    DofType type;
    union{ size_t index; double u_fixed; };

    Dof2(System2& system, size_t index);
    Dof2(System2& system, double u);

    bool operator!=(const Dof2& rhs) const;

    double u() const;
    double v() const;
    double a() const;
    double p() const;
    double& p();
};

class Node2
{
public:
    Node2(System2& system, std::array<Dof2, 3> dofs);
    Dof2 operator[](size_t i) const;

    static double distance(const Node2& node_a, const Node2& node_b);
    static double angle(const Node2& node_a, const Node2& node_b);

private:
    System2& system;
    std::array<Dof2, 3> dofs;
};
