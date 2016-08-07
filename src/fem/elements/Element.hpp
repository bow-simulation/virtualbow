#pragma once
#include "../View.hpp"
#include "../Node.hpp"

class Dof;

// Todo: Make some typedef for VectorView<Dof>

class Element
{
public:
    virtual void set_state(const VectorView<Dof> u, const VectorView<Dof> v) = 0;

    virtual void get_masses(VectorView<Dof> M) const = 0;
    virtual void get_internal_forces(VectorView<Dof> q) const = 0;
    virtual void get_tangent_stiffness(MatrixView<Dof> K) const = 0;

    virtual double get_potential_energy() const = 0;
    double get_kinetic_energy(const VectorView<Dof> v) const;
};
