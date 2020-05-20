#include "ConstraintElement.hpp"

ConstraintElement::ConstraintElement(System& system, Node node0, Node node1, double k)
    : ElementBase(system, {node0.x, node0.y, node0.phi, node1.x, node1.y}),
      k(k)
{
    double dx_abs = system.get_u(node1.x) - system.get_u(node0.x);
    double dy_abs = system.get_u(node1.y) - system.get_u(node0.y);

    dx_rel =  dx_abs*cos(system.get_u(node0.phi)) + dy_abs*sin(system.get_u(node0.phi));
    dy_rel = -dx_abs*sin(system.get_u(node0.phi)) + dy_abs*cos(system.get_u(node0.phi));
}

ConstraintElementState ConstraintElement::compute_state(const Vector<5>& u, const Vector<5>& v) const
{
    double c1 = dx_rel*cos(u(2)) - dy_rel*sin(u(2)) + u(0) - u(3);
    double c2 = dx_rel*sin(u(2)) + dy_rel*cos(u(2)) + u(1) - u(4);
    double phi = u(2);

    Vector<5> Dc1, Dc2;
    Dc1 << 1.0, 0.0, -dx_rel*sin(u(2)) - dy_rel*cos(u(2)), -1.0,  0.0;
    Dc2 << 0.0, 1.0,  dx_rel*cos(u(2)) - dy_rel*sin(u(2)),  0.0, -1.0;

    return {c1, c2, phi, Dc1, Dc2};
}

Vector<5> ConstraintElement::get_mass_matrix() const
{
    return Vector<5>::Zero();
}

Matrix<5> ConstraintElement::get_tangent_stiffness_matrix() const
{
    ConstraintElementState state = get_state();

    // Todo: Don't actually create sparse matrices DDc1, DDc2
    Matrix<5, 5> DDc1 = Matrix<5, 5>::Zero();
    DDc1(2, 2) = dy_rel*sin(state.phi) - dx_rel*cos(state.phi);

    Matrix<5, 5> DDc2 = Matrix<5, 5>::Zero();
    DDc2(2, 2) = -dy_rel*cos(state.phi) - dx_rel*sin(state.phi);

    return k*(state.Dc1*state.Dc1.transpose() + state.Dc2*state.Dc2.transpose() + state.c1*DDc1 + state.c2*DDc2);
}

Matrix<5> ConstraintElement::get_tangent_damping_matrix() const
{
    return Matrix<5>::Zero();
}

Vector<5> ConstraintElement::get_internal_forces() const
{
    ConstraintElementState state = get_state();
    return k*(state.c1*state.Dc1 + state.c2*state.Dc2);
}

double ConstraintElement::get_potential_energy() const
{
    ConstraintElementState state = get_state();
    return 0.5*k*(state.c1*state.c1 + state.c2*state.c2);
}
