#pragma once
#include "solver/fem/Element.hpp"
#include "solver/fem/Node.hpp"

#include <array>

class ConstraintElement: public Element
{
public:
    ConstraintElement(System& system, Node node0, Node node1, double k);

    void add_masses() const override;
    void add_internal_forces() const override;
    void add_tangent_stiffness() const override;
    void add_tangent_damping() const override;

    double get_potential_energy() const override;
    double get_kinetic_energy() const override;

private:
    std::array<Dof, 5> dofs;

    double k;
    double dx_rel;
    double dy_rel;
};
