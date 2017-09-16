#pragma once
#include "fem/Element.hpp"
#include "fem/Node.hpp"

struct Kinematics
{
    double e;
    double b1;
    double b2;
    Eigen::Matrix<double, 8, 1> v1;
    Eigen::Matrix<double, 8, 1> v2;
    Eigen::Matrix<double, 8, 1> Db1;
    Eigen::Matrix<double, 8, 1> Db2;
    Eigen::Matrix<double, 8, 8> Dv1;
    Eigen::Matrix<double, 8, 8> Dv2;
};

class ContactElement: public ElementInterface
{
public:
    ContactElement(Node nd0, Node nd1, Node nd2, double h0, double h1, double k);

    virtual void get_masses(VectorView<Dof> M) const override;
    virtual void get_internal_forces(VectorView<Dof> q) const override;
    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override;
    virtual double get_potential_energy() const override;
    Kinematics get_kinematics() const;

private:
    std::array<Dof, 8> dofs;

    double h0;
    double h1;
    double k;
};
