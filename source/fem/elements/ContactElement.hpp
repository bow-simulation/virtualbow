#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"
#include <array>

class ContactElement: public Element
{
public:
    ContactElement(System& system, Node nd0, Node nd1, Node nd2, double h0, double h1, double k);

    virtual void add_masses() const override;
    virtual void add_internal_forces() const override;
    virtual void add_tangent_stiffness() const override;
    virtual double get_potential_energy() const override;
    virtual double get_kinetic_energy() const override;

private:
    std::array<Node, 3> nodes;

    double h0;
    double h1;
    double k;
};
