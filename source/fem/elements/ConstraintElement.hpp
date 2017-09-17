#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class ConstraintElement: public Element
{
public:
    ConstraintElement(Node nd0, Node nd1, double k);

    virtual void update_state() override;
    virtual void get_masses(VectorView M) const override;
    virtual void get_internal_forces(VectorView q) const override;
    virtual void get_tangent_stiffness(MatrixView K) const override;
    virtual double get_potential_energy() const override;

private:
    std::array<Dof, 5> dofs;

    // Parameters
    double k;
    double dx_rel;
    double dy_rel;

    // State
    double c1;
    double c2;
};
