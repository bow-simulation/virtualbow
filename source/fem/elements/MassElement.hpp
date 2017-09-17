#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class MassElement: public Element
{
public:
    MassElement(Node nd, double m, double I = 0.0);
    void set_node(Node nd);

    virtual void update_state() override;
    virtual void get_masses(VectorView M) const override;
    virtual void get_internal_forces(VectorView q) const override;
    virtual void get_tangent_stiffness(MatrixView K) const override;
    virtual double get_potential_energy() const override;

private:
    Node node;
    double m;
    double I;
};
