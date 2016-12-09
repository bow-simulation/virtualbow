#pragma once
#include "../Element.hpp"
#include "../Node.hpp"

class MassElement: public Element
{
private:
    Node node;

    double m;
    double I;

public:
    MassElement(Node node, double m, double I = 0.0)
        : node(node), m(m), I(I)
    {

    }

    virtual void get_masses(VectorView<Dof> M) const override
    {
        M(node[0]) += m;
        M(node[1]) += m;
        M(node[2]) += I;
    }

    virtual void get_internal_forces(VectorView<Dof> q) const override
    {

    }

    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override
    {

    }

    virtual double get_potential_energy() const override
    {
        return 0.0;
    }

};
