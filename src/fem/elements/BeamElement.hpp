#pragma once
#include "Element.hpp"

#include <cmath>

class BeamElement: public Element
{
private:
    std::array<Dof, 6> dofs;
    double phi_ref_0;
    double phi_ref_1;
    double rhoA;
    double L;

    double dx;
    double dy;
    Eigen::Matrix<double, 3, 6> J;
    Eigen::Matrix<double, 3, 1> e;
    Eigen::Matrix<double, 3, 3> C;

public:
    BeamElement(Node nd0, Node nd1, double Cee, double Ckk, double Cek, double rhoA, double L)
        : dofs{{nd0.x, nd0.y, nd0.phi, nd1.x, nd1.y, nd1.phi}},
          phi_ref_0(0.0),
          phi_ref_1(0.0),
          rhoA(rhoA),
          L(L)
    {
        C << Cee,    -Cek,     Cek,
            -Cek, 4.0*Ckk, 2.0*Ckk,
             Cek, 2.0*Ckk, 4.0*Ckk;
    }

    void set_reference_angles(double phi_ref_0, double phi_ref_1)
    {
        this->phi_ref_0 = phi_ref_0;
        this->phi_ref_1 = phi_ref_1;
    }

    virtual void set_state(const VectorView<Dof> u, const VectorView<Dof> v)
    {
        dx = u(dofs[3]) - u(dofs[0]);
        dy = u(dofs[4]) - u(dofs[1]);
        double alpha = std::atan2(dy, dx);

        // Elastic coordinates
        double sin_e1 = std::sin(u(dofs[2]) + phi_ref_0 - alpha);
        double cos_e1 = std::cos(u(dofs[2]) + phi_ref_0 - alpha);
        double sin_e2 = std::sin(u(dofs[5]) + phi_ref_1 - alpha);
        double cos_e2 = std::cos(u(dofs[5]) + phi_ref_1 - alpha);

        e(0) = std::hypot(dx, dy) - L;
        e(1) = std::atan(sin_e1/cos_e1);     // Todo: Replace atan(sin(x)/cos(x)) with a cheaper function
        e(2) = std::atan(sin_e2/cos_e2);

        // Jacobian of the elastic coordinates
        double a0 = std::pow(dx*dx + dy*dy, -0.5);
        double a1 = 1.0/(dx*dx + dy*dy);

        double j0 = a0*dx;
        double j1 = a0*dy;
        double j2 = a1*dx;
        double j3 = a1*dy;

        J << -j0, -j1, 0.0, j0,  j1, 0.0,
             -j3,  j2, 1.0, j3, -j2, 0.0,
             -j3,  j2, 0.0, j3, -j2, 1.0;
    }

    virtual void get_masses(VectorView<Dof> M) const override
    {
        double alpha = 0.02;                       // Todo: Magic number
        double m = 0.5*rhoA*L;
        double I = alpha*rhoA*std::pow(L, 3);

        M(dofs[0]) += m;
        M(dofs[1]) += m;
        M(dofs[2]) += I;
        M(dofs[3]) += m;
        M(dofs[4]) += m;
        M(dofs[5]) += I;
    }

    virtual void get_internal_forces(VectorView<Dof> q) const override
    {
        q(dofs) += 1.0/L*J.transpose()*C*e;
    }

    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override
    {
        // Todo: Introduce some more variables for common expressions

        double a0 = std::pow(dx*dx + dy*dy, -0.5);
        double a1 =     1.0/(dx*dx + dy*dy);
        double a2 = std::pow(dx*dx + dy*dy, -1.5);
        double a3 = std::pow(dx*dx + dy*dy, -2.0);

        double b0 = a2*dx*dx - a0;
        double b1 = a2*dy*dy - a0;
        double b2 = a2*dx*dy;
        double b3 = 2.0*a3*dx*dx - a1;
        double b4 = 2.0*a3*dy*dy - a1;
        double b5 = 2.0*a3*dx*dy;

        Eigen::Matrix<double, 3, 6> dJ0;
        dJ0 << -b0, -b2, 0.0, b0,  b2, 0.0,
               -b5,  b3, 0.0, b5, -b3, 0.0,
               -b5,  b3, 0.0, b5, -b3, 0.0;

        Eigen::Matrix<double, 3, 6> dJ1;
        dJ1 << -b2, -b1, 0.0, b2,  b1, 0.0,
               -b4,  b5, 0.0, b4, -b5, 0.0,
               -b4,  b5, 0.0, b4, -b5, 0.0;

        Eigen::Matrix<double, 6, 6> Kn = Eigen::Matrix<double, 6, 6>::Zero();
        Kn.col(0) =  1.0/L*dJ0.transpose()*C*e;
        Kn.col(1) =  1.0/L*dJ1.transpose()*C*e;
        Kn.col(3) = -1.0/L*dJ0.transpose()*C*e;
        Kn.col(4) = -1.0/L*dJ1.transpose()*C*e;

        K(dofs) += Kn + 1.0/L*J.transpose()*C*J;
    }

    virtual double get_potential_energy() const override
    {
        return 0.5/L*e.transpose()*C*e;
    }
};

