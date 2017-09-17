#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class BeamElement: public Element
{
public:
    BeamElement(Node nd0, Node nd1, double rhoA, double L);

    void set_reference_angles(double phi_ref_0, double phi_ref_1);
    void set_stiffness(double Cee, double Ckk, double Cek);
    double get_epsilon() const;
    double get_kappa(double p) const;

    virtual void update_state() override;
    virtual void get_masses(VectorView M) const override;
    virtual void get_internal_forces(VectorView q) const override;
    virtual void get_tangent_stiffness(MatrixView K) const override;
    virtual double get_potential_energy() const override;

private:
    std::array<Dof, 6> dofs;

    // Parameters
    double phi_ref_0;
    double phi_ref_1;
    double rhoA;
    double L;
    Matrix<3, 3> C;

    // State
    Matrix<3, 1> e;
    Matrix<3, 6> J;
};

