#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"
#include <array>

class MassElement: public Element
{
public:
    MassElement(System& system, Node node, double m, double I = 0.0);
    void set_node(Node nd);

    virtual void add_masses() const override;
    virtual void add_internal_forces() const override;
    virtual void add_tangent_stiffness() const override;
    virtual void add_tangent_damping() const override;

    virtual double get_potential_energy() const override;
    virtual double get_kinetic_energy() const override;

private:
    std::array<Dof, 3> dofs;
    double m;
    double I;
};
