#pragma once
#include "Element.hpp"

class ContactElement1D: public Element
{
private:
    std::array<Node, 2> nodes;

    // Parameters
    bool one_sided;
    double k;
    double d;

    // State
    double delta_u;
    double delta_v;

public:
    ContactElement1D(Node node0, Node node1, double k, double d)
        : nodes{{node0, node1}},
          k(k),
          d(d),
          one_sided(false)
    {

    }

    // Todo: Remove, find alternative in BowModel
    ContactElement1D()
    {

    }

    void set_one_sided(bool val)
    {
        one_sided = val;
    }

    void set_stiffness(double val)
    {
        k = val;
    }

    void set_damping(double val)
    {
        d = val;
    }

    virtual void set_state(const VectorView<Dof> u, const VectorView<Dof> v)
    {
        delta_u = u(nodes[1].x) - u(nodes[0].x);
        delta_v = v(nodes[1].x) - v(nodes[0].x);
    }

    virtual void get_masses(VectorView<Dof> M) const override
    {

    }

    virtual void get_internal_forces(VectorView<Dof> q) const override
    {
        if(!one_sided || delta_u < 0.0)
        {
            double fc = k*delta_u + d*delta_v;  // Contact force

            q(nodes[0].x) -= fc;
            q(nodes[1].x) += fc;
        }
    }

    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override
    {
        if(!one_sided || delta_u < 0.0)
        {
            K(nodes[0].x, nodes[0].x) += k;
            K(nodes[1].x, nodes[1].x) += k;
            K(nodes[0].x, nodes[1].x) -= k;
        }
    }

    virtual double get_potential_energy() const override
    {
        if(!one_sided || delta_u < 0.0)
        {
            return 0.5*k*delta_u*delta_u;
        }
        else
        {
            return 0.0;
        }
    }
};
