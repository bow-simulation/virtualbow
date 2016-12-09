#pragma once
#include "View.hpp"

class Dof2;

// Todo: Make some typedef for VectorView<Dof>

class Element2
{
public:
    virtual void get_masses(VectorView<Dof2> M) const = 0;
    virtual void get_internal_forces(VectorView<Dof2> q) const = 0;
    virtual void get_tangent_stiffness(MatrixView<Dof2> K) const = 0;

    virtual double get_potential_energy() const = 0;
    double get_kinetic_energy(VectorView<Dof2> v) const;
};
