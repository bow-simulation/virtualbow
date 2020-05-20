#pragma once
#include "solver/fem/ElementBase.hpp"
#include "solver/fem/Node.hpp"
#include <array>

struct BarElementState
{
    double dx;
    double dy;
    double L_new;

    double dvx;
    double dvy;
    double L_dot;
};

class BarElement: public ElementBase<4, BarElementState>
{
public:
    BarElement(System& system, Node node0, Node node1, double L, double EA, double etaA, double rhoA);

    double get_length() const;
    void set_length(double L);
    void set_stiffness(double EA);
    void set_damping(double etaA);

    double get_normal_force() const;

    virtual BarElementState compute_state(const Vector<4>& u, const Vector<4>& v) const override;
    virtual Vector<4> get_mass_matrix() const override;
    virtual Matrix<4> get_tangent_stiffness_matrix() const override;
    virtual Matrix<4> get_tangent_damping_matrix() const override;
    virtual Vector<4> get_internal_forces() const override;
    virtual double get_potential_energy() const override;

private:
    double L;
    double EA;
    double etaA;
    double rhoA;
};
