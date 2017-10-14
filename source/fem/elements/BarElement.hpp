#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class BarElement: public Element
{
public:
    BarElement(System& system, Node nd0, Node nd1, double L, double EA, double etaA, double rhoA);

    double get_length() const;
    void set_length(double val);
    double get_normal_force() const;

    virtual void add_masses() const override;
    virtual void add_internal_forces() const override;
    virtual void add_tangent_stiffness() const override;
    virtual double get_potential_energy() const override;
    virtual double get_kinetic_energy() const override;

private:
    std::array<Node, 2> nodes;

    double L;
    double EA;
    double etaA;
    double rhoA;
};
