#pragma once
#include "solver/fem/Element.hpp"
#include "solver/fem/Node.hpp"
#include "solver/numerics/Eigen.hpp"

class BeamElement: public Element
{
public:
    BeamElement(System& system, Node node0, Node node1, double rhoA, double L);

    void set_reference_angles(double phi_ref_0, double phi_ref_1);
    void set_stiffness(double Cee, double Ckk, double Cek);
    void set_damping(double beta);

    double get_epsilon() const;
    double get_kappa(double p) const;

    virtual void add_masses() const override;
    virtual void add_internal_forces() const override;
    virtual void add_tangent_stiffness() const override;
    virtual void add_tangent_damping() const override;

    virtual double get_potential_energy() const override;
    virtual double get_kinetic_energy() const override;

private:
    std::array<Dof, 6> dofs;

    Vector<6> M;
    Matrix<3, 3> K;
    Matrix<6, 6> D;

    double phi_ref_0;
    double phi_ref_1;
    double L;

    Vector<3> get_e() const;
    Matrix<3, 6> get_J() const;
};
