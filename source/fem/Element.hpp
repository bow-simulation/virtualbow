#pragma once
#include "fem/View.hpp"
#include "fem/Node.hpp"

class System;

class Element
{
public:
    Element(System& system)
        : system(system)
    {

    }

    virtual ~Element()
    {

    }

    virtual void get_masses(VectorView<Dof> M) const = 0;
    virtual void get_internal_forces(VectorView<Dof> q) const = 0;
    virtual void get_tangent_stiffness(MatrixView<Dof> K) const = 0;

    virtual double get_potential_energy() const = 0;
    virtual double get_kinetic_energy(VectorView<Dof> v) const
    {
        double e_kin = 0.0;
        get_masses(VectorView<Dof>(nullptr, nullptr, [&](Dof dof, double val)
        {
            e_kin += 0.5*val*std::pow(v(dof), 2);
        }));

        return e_kin;
    }

protected:
    System& system;
};
