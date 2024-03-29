#pragma once
#include "solver/fem/Element.hpp"
#include "solver/fem/Node.hpp"
#include <array>

class BarElement: public Element
{
public:
    BarElement(System& system, Node node0, Node node1, double L, double EA, double etaA, double rhoA);

    double get_length() const;
    void set_length(double L);
    void set_stiffness(double EA);
    void set_damping(double etaA);

    double get_normal_force() const;

    void add_masses() const override;
    void add_internal_forces() const override;
    void add_tangent_stiffness() const override;
    void add_tangent_damping() const override;

    double get_potential_energy() const override;
    double get_kinetic_energy() const override;

private:
    std::array<Dof, 4> dofs;

    double L;
    double EA;
    double etaA;
    double rhoA;
};
