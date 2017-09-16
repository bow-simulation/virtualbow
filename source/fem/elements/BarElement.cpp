#include "BarElement.hpp"

BarElement::BarElement(Node nd0, Node nd1, double L, double EA, double rhoA)
    : nodes{nd0, nd1},
      L(L),
      EA(EA),
      rhoA(rhoA)
{

}

double BarElement::get_length() const
{
    return L;
}

void BarElement::set_length(double val)
{
    L = val;
}

double BarElement::get_normal_force() const
{
    return EA/L*(L_new - L);
}

void BarElement::update_state()
{
    dx = nodes[1][0].u() - nodes[0][0].u();
    dy = nodes[1][1].u() - nodes[0][1].u();
    L_new = hypot(dx, dy);
}

void BarElement::get_masses(VectorView<Dof> M) const
{
    double m = 0.5*rhoA*L;

    M(nodes[0][0]) += m;
    M(nodes[0][1]) += m;
    M(nodes[1][0]) += m;
    M(nodes[1][1]) += m;
}

void BarElement::get_internal_forces(VectorView<Dof> q) const
{
    double N = get_normal_force();

    q(nodes[0][0]) -= N*dx/L_new;
    q(nodes[0][1]) -= N*dy/L_new;
    q(nodes[1][0]) += N*dx/L_new;
    q(nodes[1][1]) += N*dy/L_new;
}

void BarElement::get_tangent_stiffness(MatrixView<Dof> K) const
{
    double c0 = EA*(L_new - L)/(L_new*L);
    double c1 = EA/pow(L_new, 3);

    // col 0
    K(nodes[0][0], nodes[0][0]) += c1*dx*dx + c0;

    // col 1
    K(nodes[0][0], nodes[0][1]) += c1*dx*dy;
    K(nodes[0][1], nodes[0][1]) += c1*dy*dy + c0;

    // col 2
    K(nodes[0][0], nodes[1][0]) += -c1*dx*dx - c0;
    K(nodes[0][1], nodes[1][0]) += -c1*dx*dy;
    K(nodes[1][0], nodes[1][0]) +=  c1*dx*dx + c0;

    // col 3
    K(nodes[0][0], nodes[1][1]) += -c1*dx*dy;
    K(nodes[0][1], nodes[1][1]) += -c1*dy*dy - c0;
    K(nodes[1][0], nodes[1][1]) +=  c1*dx*dy;
    K(nodes[1][1], nodes[1][1]) +=  c1*dy*dy + c0;
}

double BarElement::get_potential_energy() const
{
    return 0.5*EA/L*pow(L_new - L, 2);
}
