#pragma once
#include "solver/fem/ElementBase.hpp"
#include "solver/fem/Node.hpp"

#include <array>

struct ConstraintElementState
{
    double c1;
    double c2;
    double phi;
    Vector<5> Dc1;
    Vector<5> Dc2;
};

class ConstraintElement: public ElementBase<5, ConstraintElementState>
{
public:
    ConstraintElement(System& system, Node node0, Node node1, double k);

    virtual ConstraintElementState compute_state(const Vector<5>& u, const Vector<5>& v) const override;
    virtual Vector<5> get_mass_matrix() const override;
    virtual Matrix<5> get_tangent_stiffness_matrix() const override;
    virtual Matrix<5> get_tangent_damping_matrix() const override;
    virtual Vector<5> get_internal_forces() const override;
    virtual double get_potential_energy() const override;

private:
    double k;
    double dx_rel;
    double dy_rel;
};
