#pragma once
#include "../Element2.hpp"
#include "../Node2.hpp"

class MassElement2: public Element2
{
private:
    Node2 node;

    double m;
    double I;

public:
    MassElement2(Node2 node, double m, double I = 0.0)
        : node(node), m(m), I(I)
    {

    }

    virtual void get_masses(VectorView<Dof2> M) const override
    {
        M(node[0]) += m;
        M(node[1]) += m;
        M(node[2]) += I;
    }

    virtual void get_internal_forces(VectorView<Dof2> q) const override
    {

    }

    virtual void get_tangent_stiffness(MatrixView<Dof2> K) const override
    {

    }

    virtual double get_potential_energy() const override
    {
        return 0.0;
    }

};
