#include "BeamElement.hpp"
#include "solver/fem/System.hpp"

#include <iostream>
#include <cstdlib>

#include <iostream>

BeamElement::BeamElement(System& system, Node node0, Node node1, const Matrix<6, 6>& K, const Vector<6>& M)
    : Element(system),
      dofs{node0.x, node0.y, node0.phi, node1.x, node1.y, node1.phi},
      M(M), D(Matrix<6, 6>::Zero())
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[3]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[4]) - system.get_u(dofs[1]);
    double phi = std::atan2(dy, dx);

    Matrix<3, 3> T;
    T << cos(phi), 0.0, 0.0,
              0.0, 1.0, 0.0,
              0.0, 0.0, 1.0;

    e_ref << dx/cos(phi),
             system.get_u(dofs[2]) - phi,
             system.get_u(dofs[5]) - phi;

    Ke = T.transpose()*(Matrix<3, 3>() << K(3, 3), K(3, 2), K(3, 5),
                                          K(2, 3), K(2, 2), K(2, 5),
                                          K(5, 3), K(5, 2), K(5, 5)).finished()*T;
}

void BeamElement::set_damping(double beta)
{
    D =  beta*M(0, 0)*Matrix<6, 6>::Identity();
}

void BeamElement::add_masses() const
{
    system->add_M(dofs, M);
}

void BeamElement::add_internal_forces() const
{
    Matrix<3, 6> J = get_J();
    Vector<3> e = get_e();
    Vector<6> v = system->get_v(dofs);

    system->add_q(dofs, J.transpose()*Ke*e + D*v);
}

void BeamElement::add_tangent_stiffness() const
{
    Vector<3> e = get_e();
    Matrix<3, 6> J = get_J();

    double dx = system->get_u(dofs[3]) - system->get_u(dofs[0]);
    double dy = system->get_u(dofs[4]) - system->get_u(dofs[1]);

    double a0 = pow(dx*dx + dy*dy, -0.5);
    double a1 = pow(dx*dx + dy*dy, -1.0);
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
    Kn.col(0) =  dJ0.transpose()*Ke*e;
    Kn.col(1) =  dJ1.transpose()*Ke*e;
    Kn.col(3) = -dJ0.transpose()*Ke*e;
    Kn.col(4) = -dJ1.transpose()*Ke*e;

    system->add_K(dofs, Kn + J.transpose()*Ke*J);
}

void BeamElement::add_tangent_damping() const
{
    system->add_D(dofs, D);
}

double BeamElement::get_potential_energy() const
{
    Vector<3> e = get_e();
    return 0.5*e.transpose()*Ke*e;
}

double BeamElement::get_kinetic_energy() const
{
    Vector<6> v = system->get_v(dofs);
    return 0.5*v.transpose()*M.asDiagonal()*v;
}

Vector<3> BeamElement::get_e() const
{
    double dx = system->get_u(dofs[3]) - system->get_u(dofs[0]);
    double dy = system->get_u(dofs[4]) - system->get_u(dofs[1]);
    double phi = std::atan2(dy, dx);

    // Elastic coordinates
    double sin_e1 = std::sin(system->get_u(dofs[2]) - e_ref[1] - phi);
    double cos_e1 = std::cos(system->get_u(dofs[2]) - e_ref[1] - phi);
    double sin_e2 = std::sin(system->get_u(dofs[5]) - e_ref[2] - phi);
    double cos_e2 = std::cos(system->get_u(dofs[5]) - e_ref[2] - phi);

    // Todo: Replace atan(sin(x)/cos(x)) with a cheaper function
    return { std::hypot(dx, dy) - e_ref[0],
             std::atan(sin_e1/cos_e1),
             std::atan(sin_e2/cos_e2) };
}

Matrix<3, 6> BeamElement::get_J() const
{
    double dx = system->get_u(dofs[3]) - system->get_u(dofs[0]);
    double dy = system->get_u(dofs[4]) - system->get_u(dofs[1]);

    double a0 = pow(dx*dx + dy*dy, -0.5);
    double a1 = pow(dx*dx + dy*dy, -1.0);

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
