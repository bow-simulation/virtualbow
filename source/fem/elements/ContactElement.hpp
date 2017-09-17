#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class ContactElement: public Element
{
public:
    ContactElement(Node nd0, Node nd1, Node nd2, double h0, double h1, double k);

    virtual void update_state() override;
    virtual void get_masses(VectorView M) const override;
    virtual void get_internal_forces(VectorView q) const override;
    virtual void get_tangent_stiffness(MatrixView K) const override;
    virtual double get_potential_energy() const override;

private:
    std::array<Dof, 8> dofs;

    // Parameters
    double h0;
    double h1;
    double k;

    // State
    double e;
    Vector<8> De;
    Matrix<8> DDe;
};
