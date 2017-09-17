#include "ContactElement.hpp"
#include "numerics/Math.hpp"

ContactElement::ContactElement(Node node0, Node node1, Node node2, double h0, double h1, double k)
    : dofs{node0[0], node0[1], node0[2], node1[0], node1[1], node1[2], node2[0], node2[1]},
      h0(h0), h1(h1), k(k)
{

}

#include <iostream>

void ContactElement::update_state()
{
    Vector<2> P0{dofs[0].u(), dofs[1].u()};
    Vector<2> P1{dofs[3].u(), dofs[4].u()};
    Vector<2> P2{dofs[6].u(), dofs[7].u()};
    Vector<2> Q0{dofs[0].u() + h0*sin(dofs[2].u()), dofs[1].u() - h0*cos(dofs[2].u())};
    Vector<2> Q1{dofs[3].u() + h1*sin(dofs[5].u()), dofs[4].u() - h1*cos(dofs[5].u())};

    // Tests if the triangle {a, b, c} is oriented counter-clockwise
    auto positive = [](auto a, auto b, auto c)
    {
        return (b(0) - a(0))*(c(1) - a(1)) - (b(1) - a(1))*(c(0) - a(0)) > 0;
    };

    // If no contact, set kinematic expressions to zero and return
    if(!positive(P2, P0, Q0) || !positive(P2, P1, P0) || !positive(P2, Q0, Q1) || !positive(P2, Q1, P1))
    {
        e = 0.0;
        De.setZero();
        DDe.setZero();
        return;
    }

    // Contact: Calculate kinematic expressions

    // 1. Penetration e

    double a1 = dofs[3].u() - dofs[0].u() - h0*sin(dofs[2].u()) + h1*sin(dofs[5].u());
    double a2 = dofs[4].u() - dofs[1].u() + h0*cos(dofs[2].u()) - h1*cos(dofs[5].u());
    double a3 = dofs[6].u() - dofs[0].u() - h0*sin(dofs[2].u());
    double a4 = dofs[7].u() - dofs[1].u() + h0*cos(dofs[2].u());

    e = (a1*a4 - a2*a3)/hypot(a1, a2);

    // 2. First derivative of e

    Vector<8> Da1, Da2, Da3, Da4;
    Da1 << -1.0, 0.0, -h0*cos(dofs[2].u()), 1.0, 0.0, h1*cos(dofs[5].u()), 0.0, 0.0;
    Da2 << 0.0, -1.0, -h0*sin(dofs[2].u()), 0.0, 1.0, h1*sin(dofs[5].u()), 0.0, 0.0;
    Da3 << -1.0, 0.0, -h0*cos(dofs[2].u()), 0.0, 0.0, 0.0, 1.0, 0.0;
    Da4 << 0.0, -1.0, -h0*sin(dofs[2].u()), 0.0, 0.0, 0.0, 0.0, 1.0;

    double b1 = 1.0/hypot(a1, a2);
    double b2 = (a2*a3 - a1*a4)/pow(a1*a1 + a2*a2, 1.5);

    auto v1 = a4*Da1 - a3*Da2 - a2*Da3 + a1*Da4;
    auto v2 = a1*Da1 + a2*Da2;

    De = b1*v1 + b2*v2;

    // 3. Second derivative of e
    // Todo: Don't actually create sparse matrices DDa1 ... DDa4

    Matrix<8> DDa1 = Matrix<8>::Zero();
    DDa1(2, 2) = h0*sin(dofs[2].u());
    DDa1(5, 5) = -h1*sin(dofs[5].u());

    Matrix<8> DDa2 = Matrix<8>::Zero();
    DDa2(2, 2) = -h0*cos(dofs[2].u());
    DDa2(5, 5) = h1*cos(dofs[5].u());

    Matrix<8> DDa3 = Matrix<8>::Zero();
    DDa3(2, 2) = h0*sin(dofs[2].u());

    Matrix<8> DDa4 = Matrix<8>::Zero();
    DDa4(2, 2) = -h0*cos(dofs[2].u());

    auto Db1 = -(a1*Da1 + a2*Da2)/pow(a1*a1 + a2*a2, 1.5);

    auto Db2 = hypot(a1, a2)/(pow(a1, 6) + 3.0*pow(a1, 4)*pow(a2, 2) + 3*pow(a1, 2)*pow(a2, 4) + pow(a2, 6))
             * ((2.0*a1*a1*a4 - a2*a2*a4 - 3.0*a1*a2*a3)*Da1
              + (a1*a1*a3 - 2.0*a2*a2*a3 + 3.0*a1*a2*a4)*Da2
              + (a2*a2*a2 + a1*a1*a2)*Da3
              + (-a1*a1*a1 - a1*a2*a2)*Da4);

    auto Dv1 = Da4*Da1.transpose() - Da3*Da2.transpose()
             - Da2*Da3.transpose() + Da1*Da4.transpose()
             + a4*DDa1 - a3*DDa2 - a2*DDa3 + a1*DDa4;

    auto Dv2 = Da1*Da1.transpose() + Da2*Da2.transpose()
             + a1*DDa1 + a2*DDa2;

    DDe = Db1*v1.transpose() + Db2*v2.transpose() + b1*Dv1 + b2*Dv2;
}

void ContactElement::get_masses(VectorView<Dof> M) const
{

}

void ContactElement::get_internal_forces(VectorView<Dof> q) const
{
    q(dofs) += k*e*De;
}

void ContactElement::get_tangent_stiffness(MatrixView<Dof> K) const
{
    K(dofs) += k*(De*De.transpose() + e*DDe);
}

double ContactElement::get_potential_energy() const
{
    return 0.5*k*e*e;
}
