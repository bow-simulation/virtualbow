#pragma once
#include "Element.hpp"

// Todo: Generalise for other directions
class ContactElement1D: public Element
{
private:
    Dof dof_upper;
    Dof dof_lower;

    // Parameters
    bool one_sided;
    double k;
    double d;

    // State
    double delta_u;
    double delta_v;

public:
    ContactElement1D(Dof dof_upper, Dof dof_lower, double k, double d)
        : dof_upper(dof_upper),
          dof_lower(dof_lower),
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

    virtual void set_state(const VectorView<Dof> u, const VectorView<Dof> v) override
    {
        delta_u = u(dof_upper) - u(dof_lower);
        delta_v = v(dof_upper) - v(dof_lower);

        Element::set_state(u, v);
    }

    virtual void get_masses(VectorView<Dof> M) const override
    {

    }

    virtual void get_internal_forces(VectorView<Dof> q) const override
    {
        if(!one_sided || delta_u < 0.0)
        {
            double fc = k*delta_u + d*delta_v;  // Contact force

            q(dof_lower) -= fc;
            q(dof_upper) += fc;
        }
    }

    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override
    {
        if(!one_sided || delta_u < 0.0)
        {
            K(dof_lower, dof_lower) += k;
            K(dof_upper, dof_upper) += k;
            K(dof_lower, dof_upper) -= k;
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
