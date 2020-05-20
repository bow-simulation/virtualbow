#pragma once
#include "solver/fem/ElementBase.hpp"
#include "solver/fem/Node.hpp"
#include "solver/numerics/Eigen.hpp"

struct BeamElementState
{
    double dx;
    double dy;
    double a0;
    double a1;
    Vector<6> v;
    Vector<3> e;
    Matrix<3, 6> J;
};

class BeamElement: public ElementBase<6, BeamElementState>
{
public:
    BeamElement(System& system, Node node0, Node node1, const Matrix<6, 6>& K, const Vector<6>& M);
    void set_damping(double beta);

    virtual BeamElementState compute_state(const Vector<6>& u, const Vector<6>& v) const override;
    virtual Vector<6> get_mass_matrix() const override;
    virtual Matrix<6> get_tangent_stiffness_matrix() const override;
    virtual Matrix<6> get_tangent_damping_matrix() const override;
    virtual Vector<6> get_internal_forces() const override;
    virtual double get_potential_energy() const override;

    Matrix<3, 3> Ke;
    Vector<6> M;
    Matrix<6, 6> D;
    Vector<3> e_ref;
};
