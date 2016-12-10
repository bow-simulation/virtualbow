#pragma once
#include "View.hpp"

class Dof;

// Todo: Make some typedef for VectorView<Dof>

class Element
{
public:
    virtual void get_masses(VectorView<Dof> M) const = 0;
    virtual void get_internal_forces(VectorView<Dof> q) const = 0;
    virtual void get_tangent_stiffness(MatrixView<Dof> K) const = 0;

    virtual double get_potential_energy() const = 0;
    virtual double get_kinetic_energy(VectorView<Dof> v) const;
};
