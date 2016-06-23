#pragma once
#include "../View.hpp"
#include "../Node.hpp"

class Dof;

class Element
{
public:
    virtual void get_masses(VectorView<Dof> M) const = 0;                     // Todo: Make some typedef for VectorView<Dof>
    virtual void get_internal_forces(const VectorView<Dof> u, const VectorView<Dof> v, VectorView<Dof> q) const = 0;

    virtual double get_potential_energy(const VectorView<Dof> u) const = 0;
    virtual double get_kinetic_energy(const VectorView<Dof> v) const;
};
