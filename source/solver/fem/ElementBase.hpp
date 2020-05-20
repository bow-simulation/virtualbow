#pragma once
#include "Element.hpp"
#include "System.hpp"
#include "Node.hpp"
#include "solver/numerics/Eigen.hpp"
#include <array>
#include <limits>

template<unsigned N, class State>
class ElementBase: public Element
{
public:
    ElementBase(System& system, const std::array<Dof, N>& dofs)
        : Element(system),    // Todo: Remove
          system(&system),
          dofs(dofs),
          u(Vector<N>::Constant(std::numeric_limits<double>::quiet_NaN())),
          v(Vector<N>::Constant(std::numeric_limits<double>::quiet_NaN()))
    {

    }

    // Methods to override by child classes

    virtual State compute_state(const Vector<N>& u, const Vector<N>& v) const = 0;    // Todo: Make protected?
    virtual Vector<N> get_mass_matrix() const = 0;
    virtual Matrix<N> get_tangent_stiffness_matrix() const = 0;
    virtual Matrix<N> get_tangent_damping_matrix() const = 0;
    virtual Vector<N> get_internal_forces() const = 0;
    virtual double get_potential_energy() const = 0;

    // Implementations
    // Todo: Make private?

    const State& get_state() const
    {
        Vector<N> u_new = system->get_u(dofs);
        Vector<N> v_new = system->get_v(dofs);

        if((u_new != u) || (v_new != v))
        {
            state = compute_state(u_new, v_new);
            u = u_new;
            v = v_new;
        }

        return state;
    }

    virtual void add_masses() const override final
    {
        system->add_M(dofs, get_mass_matrix());
    }

    virtual void add_internal_forces() const override final
    {
        system->add_q(dofs, get_internal_forces());
    }

    virtual void add_tangent_stiffness() const override final
    {
        system->add_K(dofs, get_tangent_stiffness_matrix());
    }

    virtual void add_tangent_damping() const override final
    {
        system->add_D(dofs, get_tangent_damping_matrix());
    }

    virtual double get_kinetic_energy() const override final
    {
        Vector<N> v = system->get_v(dofs);
        return 0.5*v.transpose()*get_mass_matrix().asDiagonal()*v;
    }

private:
    System* system;
    std::array<Dof, N> dofs;
    mutable State state;
    mutable Vector<N> u;
    mutable Vector<N> v;
};
