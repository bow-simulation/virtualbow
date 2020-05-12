#pragma once
#include "solver/fem/Element.hpp"
#include "solver/fem/Node.hpp"
#include "solver/numerics/Eigen.hpp"

class BeamElement: public Element
{
public:
    BeamElement(System& system, Node node0, Node node1, const Matrix<6, 6>& K, const Vector<6>& M);
    void set_damping(double beta);

    virtual void add_masses() const override;
    virtual void add_internal_forces() const override;
    virtual void add_tangent_stiffness() const override;
    virtual void add_tangent_damping() const override;

    virtual double get_potential_energy() const override;
    virtual double get_kinetic_energy() const override;

private:
    std::array<Dof, 6> dofs;

    Matrix<3, 3> Ke;
    Vector<6> M;
    Matrix<6, 6> D;
    Vector<3> e_ref;

    Vector<3> get_e() const;
    Matrix<3, 6> get_J() const;
};
