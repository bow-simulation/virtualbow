#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class ContactElement: public Element
{
public:
    ContactElement(System& system, Node nd0, Node nd1, Node nd2, double h0, double h1, double k);

    virtual void get_masses(VectorView<Dof> M) const override;
    virtual void get_internal_forces(VectorView<Dof> q) const override;
    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override;
    virtual double get_potential_energy() const override;

private:
    std::array<Node, 3> nodes;

    double h0;
    double h1;
    double k;
};
