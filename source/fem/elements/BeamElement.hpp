#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

class BeamElement: public ElementInterface
{
public:
    BeamElement(Node nd0, Node nd1, double rhoA, double L);

    void set_reference_angles(double phi_ref_0, double phi_ref_1);
    void set_stiffness(double Cee, double Ckk, double Cek);
    double get_epsilon(double p) const;
    double get_kappa(double p) const;

    virtual void get_masses(VectorView<Dof> M) const override;
    virtual void get_internal_forces(VectorView<Dof> q) const override;
    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override;
    virtual double get_potential_energy() const override;

private:
    std::array<Dof, 6> dofs;
    double phi_ref_0;
    double phi_ref_1;
    double rhoA;
    double L;

    Eigen::Matrix<double, 3, 3> C;

    Eigen::Matrix<double, 3, 1> get_e() const;
    Eigen::Matrix<double, 3, 6> get_J() const;

};

