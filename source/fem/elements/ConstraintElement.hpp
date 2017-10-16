#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

#include <array>

class ConstraintElement: public Element
{
public:
    ConstraintElement(System& system, Node node0, Node node1, double k);

    virtual void add_masses() const override;
    virtual void add_internal_forces() const override;
    virtual void add_tangent_stiffness() const override;

    virtual double get_potential_energy() const override;
    virtual double get_kinetic_energy() const override;

private:
    std::array<Dof, 5> dofs;

    double k;
    double dx_rel;
    double dy_rel;
};
