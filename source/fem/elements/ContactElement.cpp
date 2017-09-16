#include "ContactElement.hpp"
#include "numerics/Math.hpp"

ContactElement::ContactElement(Node node0, Node node1, Node node2, double h0, double h1, double k)
    : dofs{node0[0], node0[1], node0[2], node1[0], node1[1], node1[2], node2[0], node2[1]},
      h0(h0), h1(h1), k(k)
{

}

void ContactElement::get_masses(VectorView<Dof> M) const
{

}

void ContactElement::get_internal_forces(VectorView<Dof> q) const
{
    Kinematics kin = get_kinematics();
    q(dofs) += k*kin.e*(kin.b1*kin.v1 + kin.b2*kin.v2);
}

void ContactElement::get_tangent_stiffness(MatrixView<Dof> K) const
{
    Kinematics kin = get_kinematics();
    Vector<8> De = kin.b1*kin.v1 + kin.b2*kin.v2;
    Matrix<8> DDe = kin.Db1*kin.v1.transpose() + kin.Db2*kin.v2.transpose() + kin.b1*kin.Dv1 + kin.b2*kin.Dv2;

    K(dofs) += k*De*De.transpose() + k*kin.e*DDe;
}

double ContactElement::get_potential_energy() const
{
    Kinematics kin = get_kinematics();
    return 0.5*k*kin.e*kin.e;
}

Kinematics ContactElement::get_kinematics() const
{
    double x0 = dofs[0].u();
    double y0 = dofs[1].u();
    double phi0 = dofs[2].u();

    double x1 = dofs[3].u();
    double y1 = dofs[4].u();
    double phi1 = dofs[5].u();

    double x2 = dofs[6].u();
    double y2 = dofs[7].u();

    Vector<2> P0{x0, y0};
    Vector<2> P1{x1, y1};
    Vector<2> P2{x2, y2};
    Vector<2> Q0{x0 + h0*sin(phi0), y0 - h0*cos(phi0)};
    Vector<2> Q1{x1 + h1*sin(phi1), y1 - h1*cos(phi1)};

    // Test if triangle {p0, p1, p2} is oriented positively (counter-clockwise)
    auto positive = [](auto a, auto b, auto c)
    {
        return (b(0) - a(0))*(c(1) - a(1)) - (b(1) - a(1))*(c(0) - a(0)) > 0;
    };

    // Return zeros if no contact
    if(!positive(P2, P0, Q0) || !positive(P2, P1, P0) || !positive(P2, Q0, Q1) || !positive(P2, Q1, P1))
        return {0.0, 0.0, 0.0,
                Vector<8>::Zero(), Vector<8>::Zero(), Vector<8>::Zero(), Vector<8>::Zero(),
                Matrix<8>::Zero(), Matrix<8>::Zero()};

    double a1 = x1 - x0 - h0*sin(phi0) + h1*sin(phi1);
    double a2 = y1 - y0 + h0*cos(phi0) - h1*cos(phi1);
    double a3 = x2 - x0 - h0*sin(phi0);
    double a4 = y2 - y0 + h0*cos(phi0);

    double e = (a1*a4 - a2*a3)/hypot(a1, a2);
    double b1 = 1.0/hypot(a1, a2);
    double b2 = (a2*a3 - a1*a4)/pow(a1*a1 + a2*a2, 1.5);

    Vector<8> Da1;
    Da1 << -1.0, 0.0, -h0*cos(phi0), 1.0, 0.0, h1*cos(phi1), 0.0, 0.0;

    Vector<8> Da2;
    Da2 << 0.0, -1.0, -h0*sin(phi0), 0.0, 1.0, h1*sin(phi1), 0.0, 0.0;

    Vector<8> Da3;
    Da3 << -1.0, 0.0, -h0*cos(phi0), 0.0, 0.0, 0.0, 1.0, 0.0;

    Vector<8> Da4;
    Da4 << 0.0, -1.0, -h0*sin(phi0), 0.0, 0.0, 0.0, 0.0, 1.0;

    Vector<8> v1 = a4*Da1 - a3*Da2 - a2*Da3 + a1*Da4;
    Vector<8> v2 = a1*Da1 + a2*Da2;

    // Todo: Don't create sparse matrices DDa1 ... DDa4

    Matrix<8> DDa1 = Matrix<8>::Zero();
    DDa1(2, 2) = h0*sin(phi0);
    DDa1(5, 5) = -h1*sin(phi1);

    Matrix<8> DDa2 = Matrix<8>::Zero();
    DDa2(2, 2) = -h0*cos(phi0);
    DDa2(5, 5) = h1*cos(phi1);

    Matrix<8> DDa3 = Matrix<8>::Zero();
    DDa3(2, 2) = h0*sin(phi0);

    Matrix<8> DDa4 = Matrix<8>::Zero();
    DDa4(2, 2) = -h0*cos(phi0);

    Vector<8> Db1 = -(a1*Da1 + a2*Da2)/pow(a1*a1 + a2*a2, 1.5);

    Vector<8> Db2 = hypot(a1, a2)/(pow(a1, 6) + 3.0*pow(a1, 4)*pow(a2, 2) + 3*pow(a1, 2)*pow(a2, 4) + pow(a2, 6))
                                    * (  (2.0*a1*a1*a4 - a2*a2*a4 - 3.0*a1*a2*a3)*Da1
                                       + (a1*a1*a3 - 2.0*a2*a2*a3 + 3.0*a1*a2*a4)*Da2
                                       + (a2*a2*a2 + a1*a1*a2)*Da3
                                       + (-a1*a1*a1 - a1*a2*a2)*Da4);

    Matrix<8> Dv1 = Da4*Da1.transpose() - Da3*Da2.transpose()
                                    - Da2*Da3.transpose() + Da1*Da4.transpose()
                                    + a4*DDa1 - a3*DDa2 - a2*DDa3 + a1*DDa4;

    Matrix<8> Dv2 = Da1*Da1.transpose() + Da2*Da2.transpose()
                                    + a1*DDa1 + a2*DDa2;

    return {e, b1, b2, v1, v2, Db1, Db2, Dv1, Dv2};
}
