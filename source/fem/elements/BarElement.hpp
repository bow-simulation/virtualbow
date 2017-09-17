#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class BarElement: public Element
{
public:
    BarElement(Node nd0, Node nd1, double L, double EA, double rhoA);

    double get_length() const;
    void set_length(double val);
    double get_normal_force() const;

    virtual void update_state() override;
    virtual void get_masses(VectorView M) const override;
    virtual void get_internal_forces(VectorView q) const override;
    virtual void get_tangent_stiffness(MatrixView K) const override;
    virtual double get_potential_energy() const override;

private:
    std::array<Node, 2> nodes;

    // Parameters
    double L;
    double EA;
    double rhoA;

    // State
    double dx;
    double dy;
    double L_new;
};
