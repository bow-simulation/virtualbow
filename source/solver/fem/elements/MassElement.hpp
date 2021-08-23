#pragma once
#include "solver/fem/Element.hpp"
#include "solver/fem/Node.hpp"
#include <array>

class MassElement: public Element
{
public:
    MassElement(System& system, Node node, double m, double I = 0.0);
    void set_node(Node nd);

    void add_masses() const override;
    void add_internal_forces() const override;
    void add_tangent_stiffness() const override;
    void add_tangent_damping() const override;

    double get_potential_energy() const override;
    double get_kinetic_energy() const override;

private:
    std::array<Dof, 3> dofs;
    double m;
    double I;
};
