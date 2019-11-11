#include "BeamElement.hpp"
#include "fem/System.hpp"

#include <iostream>
#include <cstdlib>

BeamElement::BeamElement(System& system, Node node0, Node node1, double rhoA, double L, double Cee, double Ckk, double Cek, double beta)
    : Element(system),
      dofs{node0.x, node0.y, node0.phi, node1.x, node1.y, node1.phi},
      phi_ref_0(0.0),
      phi_ref_1(0.0),
      L(L)
{
    double m = 0.5*rhoA*L;
    double I = 0.02*rhoA*L*L*L;

    M << m, m, I, m, m, I;

    K << Cee/L,    -Cek/L,     Cek/L,
        -Cek/L, 4.0*Ckk/L, 2.0*Ckk/L,
         Cek/L, 2.0*Ckk/L, 4.0*Ckk/L;

    D = beta*K;
}

void BeamElement::set_reference_angles(double phi_ref_0, double phi_ref_1)
{
    this->phi_ref_0 = phi_ref_0;
    this->phi_ref_1 = phi_ref_1;
}

// p in [0, 1]
double BeamElement::get_epsilon() const
{
    return get_e()(0)/L;
}

// p in [0, 1]
double BeamElement::get_kappa(double p) const
{
    Vector<3> e = get_e();
    return (6.0*p - 4.0)/L*e(1) + (6.0*p - 2.0)/L*e(2);
}

void BeamElement::add_masses() const
{
    system.add_M(dofs, M);
}

void BeamElement::add_internal_forces() const
{
    Matrix<3, 6> J = get_J();
    Vector<3> e = get_e();
    Vector<6> v = system.get_v(dofs);

    system.add_q(dofs, J.transpose()*(K*e + D*J*v));
}

void BeamElement::add_tangent_stiffness() const
{
    Vector<3> e = get_e();
    Matrix<3, 6> J = get_J();

    double dx = system.get_u(dofs[3]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[4]) - system.get_u(dofs[1]);

    double a0 = pow(dx*dx + dy*dy, -0.5);
    double a1 = 1.0/(dx*dx + dy*dy);
    double a2 = pow(dx*dx + dy*dy, -1.5);
    double a3 = pow(dx*dx + dy*dy, -2.0);

    double b0 = a2*dx*dx - a0;
    double b1 = a2*dy*dy - a0;
    double b2 = a2*dx*dy;
    double b3 = 2.0*a3*dx*dx - a1;
    double b4 = 2.0*a3*dy*dy - a1;
    double b5 = 2.0*a3*dx*dy;

    Matrix<3, 6> dJ0;
    dJ0 << -b0, -b2, 0.0, b0,  b2, 0.0,
           -b5,  b3, 0.0, b5, -b3, 0.0,
           -b5,  b3, 0.0, b5, -b3, 0.0;

    Matrix<3, 6> dJ1;
    dJ1 << -b2, -b1, 0.0, b2,  b1, 0.0,
           -b4,  b5, 0.0, b4, -b5, 0.0,
           -b4,  b5, 0.0, b4, -b5, 0.0;

    Matrix<6, 6> Kn = Matrix<6, 6>::Zero();
    Kn.col(0) =  dJ0.transpose()*K*e;
    Kn.col(1) =  dJ1.transpose()*K*e;
    Kn.col(3) = -dJ0.transpose()*K*e;
    Kn.col(4) = -dJ1.transpose()*K*e;

    system.add_K(dofs, Kn + J.transpose()*K*J);
}

double BeamElement::get_potential_energy() const
{
    Vector<3> e = get_e();
    return 0.5*e.transpose()*K*e;
}

double BeamElement::get_kinetic_energy() const
{
    Vector<6> v = system.get_v(dofs);
    return 0.5*v.transpose()*M.asDiagonal()*v;
}

Vector<3> BeamElement::get_e() const
{
    double dx = system.get_u(dofs[3]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[4]) - system.get_u(dofs[1]);
    double phi = std::atan2(dy, dx);

    // Elastic coordinates
    double sin_e1 = std::sin(system.get_u(dofs[2]) + phi_ref_0 - phi);
    double cos_e1 = std::cos(system.get_u(dofs[2]) + phi_ref_0 - phi);
    double sin_e2 = std::sin(system.get_u(dofs[5]) + phi_ref_1 - phi);
    double cos_e2 = std::cos(system.get_u(dofs[5]) + phi_ref_1 - phi);

    // Todo: Replace atan(sin(x)/cos(x)) with a cheaper function
    return {std::hypot(dx, dy) - L, std::atan(sin_e1/cos_e1), std::atan(sin_e2/cos_e2)};
}

Matrix<3, 6> BeamElement::get_J() const
{
    double dx = system.get_u(dofs[3]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[4]) - system.get_u(dofs[1]);

    double a0 = std::pow(dx*dx + dy*dy, -0.5);
    double a1 = 1.0/(dx*dx + dy*dy);

    double j0 = a0*dx;
    double j1 = a0*dy;
    double j2 = a1*dx;
    double j3 = a1*dy;

    Eigen::Matrix<double, 3, 6> J;
    J << -j0, -j1, 0.0, j0,  j1, 0.0,
            -j3,  j2, 1.0, j3, -j2, 0.0,
            -j3,  j2, 0.0, j3, -j2, 1.0;

    return J;
}
