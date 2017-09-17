#include "BarElement.hpp"

BarElement::BarElement(Node nd0, Node nd1, double L, double EA, double rhoA)
    : dofs{nd0[0], nd0[1], nd1[0], nd1[1]},
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
    dx = dofs[2].u() - dofs[0].u();
    dy = dofs[3].u() - dofs[1].u();
    L_new = hypot(dx, dy);
}

void BarElement::get_masses(VectorView M) const
{
    double m = 0.5*rhoA*L;

    M(dofs[0]) += m;
    M(dofs[1]) += m;
    M(dofs[2]) += m;
    M(dofs[3]) += m;
}

void BarElement::get_internal_forces(VectorView q) const
{
    double N = get_normal_force();

    q(dofs[0]) -= N*dx/L_new;
    q(dofs[1]) -= N*dy/L_new;
    q(dofs[2]) += N*dx/L_new;
    q(dofs[3]) += N*dy/L_new;
}

void BarElement::get_tangent_stiffness(MatrixView K) const
{
    double c0 = EA*(L_new - L)/(L_new*L);
    double c1 = EA/pow(L_new, 3);

    // col 0
    K(dofs[0], dofs[0]) += c1*dx*dx + c0;

    // col 1
    K(dofs[0], dofs[1]) += c1*dx*dy;
    K(dofs[1], dofs[1]) += c1*dy*dy + c0;

    // col 2
    K(dofs[0], dofs[2]) += -c1*dx*dx - c0;
    K(dofs[1], dofs[2]) += -c1*dx*dy;
    K(dofs[2], dofs[2]) +=  c1*dx*dx + c0;

    // col 3
    K(dofs[0], dofs[3]) += -c1*dx*dy;
    K(dofs[1], dofs[3]) += -c1*dy*dy - c0;
    K(dofs[2], dofs[3]) +=  c1*dx*dy;
    K(dofs[3], dofs[3]) +=  c1*dy*dy + c0;
}

double BarElement::get_potential_energy() const
{
    return 0.5*EA/L*pow(L_new - L, 2);
}
