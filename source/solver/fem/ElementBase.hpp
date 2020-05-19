#pragma once
#include "Element.hpp"
#include "System.hpp"
#include "Node.hpp"
#include "solver/numerics/Eigen.hpp"
#include <array>

template<unsigned N>
class ElementBase: public Element
{
public:
    ElementBase(System& system, const std::array<Dof, N>& dofs)
        : Element(system),    // Todo: Remove
          system(&system),
          dofs(dofs)
    {

    }

    // Methods to override by child classes

    virtual Vector<N> get_mass_matrix() const = 0;
    virtual Matrix<N, N> get_tangent_stiffness_matrix(const Vector<N>& u) const = 0;
    virtual Matrix<N, N> get_tangent_damping_matrix(const Vector<N>& u) const = 0;

    virtual Vector<N> get_internal_forces(const Vector<N>& u, const Vector<N>& v) const = 0;
    virtual double get_potential_energy(const Vector<N>& u) const = 0;

    // Implementation of interited methods

    virtual void add_masses() const override final {
        system->add_M(dofs, get_mass_matrix());
    }

    virtual void add_internal_forces() const override final {
        system->add_q(dofs, get_internal_forces(system->get_u(dofs), system->get_v(dofs)));
    }

    virtual void add_tangent_stiffness() const override final {
        system->add_K(dofs, get_tangent_stiffness_matrix(system->get_u(dofs)));
    }

    virtual void add_tangent_damping() const override final {
        system->add_D(dofs, get_tangent_damping_matrix(system->get_u(dofs)));
    }

    virtual double get_potential_energy() const override final {
        return get_potential_energy(system->get_u(dofs));
    }

    virtual double get_kinetic_energy() const override final {
        Vector<N> v = system->get_v(dofs);
        return 0.5*v.transpose()*get_mass_matrix().asDiagonal()*v;
    }

private:
    System* system;
    std::array<Dof, N> dofs;
};
