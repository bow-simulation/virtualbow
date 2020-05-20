#include "ContactElement.hpp"
#include "solver/numerics/Geometry.hpp"
#include "solver/fem/System.hpp"

ContactForce::ContactForce(double k, double epsilon)
    : k(k), epsilon(epsilon)
{

}

double ContactForce::force(double e) const
{
    if(e <= 0.0)
    {
        return 0.0;
    }
    if(e <= epsilon)
    {
        return k/(2.0*epsilon)*e*e;
    }
    else
    {
        return k*(e-0.5*epsilon);
    }
}

double ContactForce::stiffness(double e) const
{
    if(e <= 0.0)
    {
        return 0.0;
    }
    if(e <= epsilon)
    {
        return k/epsilon*e;
    }
    else
    {
        return k;
    }
}

double ContactForce::energy(double e) const
{
    if(e <= 0.0)
    {
        return 0.0;
    }
    if(e <= epsilon)
    {
        return k/(6.0*epsilon)*e*e*e;
    }
    else
    {
        return 0.5*k*e*e - 0.5*k*epsilon*e + k/6.0*epsilon*epsilon;
    }
}

ContactElement::ContactElement(System& system, Node node0, Node node1, Node node2, double h0, double h1, ContactForce f)
    : ElementBase(system, {node0.x, node0.y, node0.phi, node1.x, node1.y, node1.phi, node2.x, node2.y}),
      h0(h0), h1(h1), f(f)
{

}

ContactElementState ContactElement::compute_state(const Vector<8>& u, const Vector<8>& v) const
{
    Vector<2> P0{u(0), u(1)};
    Vector<2> P1{u(3), u(4)};
    Vector<2> P2{u(6), u(7)};

    // Bounding box check, x-direction
    if(P2(0) < std::min(P0(0) - h0, P1(0) - h1) || P2(0) > std::max(P0(0) + h0, P1(0) + h1)) {
        return ContactElementState();
    }

    // Bounding box check, y-direction
    if(P2(1) < std::min(P0(1) - h0, P1(1) - h1) || P2(1) > std::max(P0(1) + h0, P1(1) + h1)) {
        return ContactElementState();
    }

    // Exact contact check
    Vector<2> Q0{u(0) + h0*sin(u(2)), u(1) - h0*cos(u(2))};
    Vector<2> Q1{u(3) + h1*sin(u(5)), u(4) - h1*cos(u(5))};

    if(get_orientation(P2, P0, Q0) == Orientation::LeftHanded ||
       get_orientation(P2, P1, P0) == Orientation::LeftHanded ||
       get_orientation(P2, Q0, Q1) == Orientation::LeftHanded ||
       get_orientation(P2, Q1, P1) == Orientation::LeftHanded)
    {
        return ContactElementState();
    }

    // Contact: Calculate kinematic expressions

    // Penetration e

    double a1 = u(3) - u(0) - h0*sin(u(2)) + h1*sin(u(5));
    double a2 = u(4) - u(1) + h0*cos(u(2)) - h1*cos(u(5));
    double a3 = u(6) - u(0) - h0*sin(u(2));
    double a4 = u(7) - u(1) + h0*cos(u(2));

    double e = (a1*a4 - a2*a3)/hypot(a1, a2);

    // First derivative of e

    Vector<8> Da1, Da2, Da3, Da4;
    Da1 << -1.0, 0.0, -h0*cos(u(2)), 1.0, 0.0, h1*cos(u(5)), 0.0, 0.0;
    Da2 << 0.0, -1.0, -h0*sin(u(2)), 0.0, 1.0, h1*sin(u(5)), 0.0, 0.0;
    Da3 << -1.0, 0.0, -h0*cos(u(2)), 0.0, 0.0,          0.0, 1.0, 0.0;
    Da4 << 0.0, -1.0, -h0*sin(u(2)), 0.0, 0.0,          0.0, 0.0, 1.0;

    double b1 = 1.0/hypot(a1, a2);
    double b2 = (a2*a3 - a1*a4)/pow(a1*a1 + a2*a2, 1.5);

    Vector<8> v1 = a4*Da1 - a3*Da2 - a2*Da3 + a1*Da4;
    Vector<8> v2 = a1*Da1 + a2*Da2;
    Vector<8> De = b1*v1 + b2*v2;

    return {u, e, a1, a2, a3, a4, b1, b2, v1, v2, Da1, Da2, Da3, Da4, De};
}

Vector<8> ContactElement::get_mass_matrix() const
{
    return Vector<8>::Zero();
}

Matrix<8> ContactElement::get_tangent_stiffness_matrix() const
{
    ContactElementState state = get_state();

    Vector<8> u = state.u;

    Vector<8> Da1 = state.Da1;
    Vector<8> Da2 = state.Da2;
    Vector<8> Da3 = state.Da3;
    Vector<8> Da4 = state.Da4;

    double a1 = state.a1;
    double a2 = state.a2;
    double a3 = state.a3;
    double a4 = state.a4;

    double b1 = state.b1;
    double b2 = state.b2;

    Vector<8> v1 = state.v1;
    Vector<8> v2 = state.v2;

    // Second derivative of e
    // Todo: Don't actually create sparse matrices DDa1 ... DDa4

    Matrix<8, 8> DDa1 = Matrix<8, 8>::Zero();
    DDa1(2, 2) = h0*sin(u(2));
    DDa1(5, 5) = -h1*sin(u(5));

    Matrix<8, 8> DDa2 = Matrix<8, 8>::Zero();
    DDa2(2, 2) = -h0*cos(u(2));
    DDa2(5, 5) = h1*cos(u(5));

    Matrix<8, 8> DDa3 = Matrix<8, 8>::Zero();
    DDa3(2, 2) = h0*sin(u(2));

    Matrix<8, 8> DDa4 = Matrix<8, 8>::Zero();
    DDa4(2, 2) = -h0*cos(u(2));

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

    Matrix<8, 8> DDe = Db1*v1.transpose() + Db2*v2.transpose() + b1*Dv1 + b2*Dv2;

    if(state.e != 0.0) {
        return f.stiffness(state.e)*state.De*state.De.transpose() + f.force(state.e)*state.e*DDe;
    } else {
        return Matrix<8>::Zero();
    }
}

Matrix<8> ContactElement::get_tangent_damping_matrix() const
{
    return Matrix<8>::Zero();
}

Vector<8> ContactElement::get_internal_forces() const
{
    ContactElementState state = get_state();

    if(state.e != 0.0) {
        return f.force(state.e)*state.De;
    } else {
        return Vector<8>::Zero();
    }
}

double ContactElement::get_potential_energy() const
{
    ContactElementState state = get_state();

    if(state.e != 0.0) {
        return f.energy(state.e);
    } else {
        return 0.0;
    }
}
