#pragma once
#include "solver/fem/Element.hpp"
#include "solver/fem/Node.hpp"
#include "solver/numerics/EigenTypes.hpp"
#include <array>

struct ContactForce
{
public:
    ContactForce(double k, double epsilon);
    double force(double e) const;
    double stiffness(double e) const;
    double energy(double e) const;

private:
    double k;
    double epsilon;
};

class ContactElement: public Element
{
public:
    ContactElement(System& system, Node node0, Node node1, Node node2, double h0, double h1, ContactForce f);

    void add_masses() const override;
    void add_internal_forces() const override;
    void add_tangent_stiffness() const override;
    void add_tangent_damping() const override;

    double get_potential_energy() const override;
    double get_kinetic_energy() const override;

private:
    std::array<Dof, 8> dofs;

    double h0;
    double h1;
    ContactForce f;

    struct State
    {
        double e;
        Vector<8> De;
        Matrix<8, 8> DDe;
    };

    State get_state() const;
};
