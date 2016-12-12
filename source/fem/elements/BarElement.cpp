#include "BarElement.hpp"

BarElement::BarElement(Node nd0, Node nd1, double L, double EA, double etaA, double rhoA)
    : nodes{nd0, nd1},
      L(L),
      EA(EA),
      etaA(etaA),
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
    // Todo: Code duplication
    double dx = nodes[1][0].u() - nodes[0][0].u();
    double dy = nodes[1][1].u() - nodes[0][1].u();
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(nodes[1][0].v() - nodes[0][0].v() + dy*nodes[1][1].v() - nodes[0][1].v()));

    return EA/L*(L_new - L) + etaA/L*L_dot;
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
    // Todo: Code duplication
    double dx = nodes[1][0].u() - nodes[0][0].u();
    double dy = nodes[1][1].u() - nodes[0][1].u();
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(nodes[1][0].v() - nodes[0][0].v() + dy*nodes[1][1].v() - nodes[0][1].v()));

    double N = get_normal_force();

    q(nodes[0][0]) -= N*dx/L_new;
    q(nodes[0][1]) -= N*dy/L_new;
    q(nodes[1][0]) += N*dx/L_new;
    q(nodes[1][1]) += N*dy/L_new;
}

void BarElement::get_tangent_stiffness(MatrixView<Dof> K) const
{
    // Todo: Code duplication
    double dx = nodes[1][0].u() - nodes[0][0].u();
    double dy = nodes[1][1].u() - nodes[0][1].u();
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(nodes[1][0].v() - nodes[0][0].v() + dy*nodes[1][1].v() - nodes[0][1].v()));

    double c0 = EA*(L_new - L)/(L_new*L);
    double c1 = EA/std::pow(L_new, 3);

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
    // Todo: Code duplication
    double dx = nodes[1][0].u() - nodes[0][0].u();
    double dy = nodes[1][1].u() - nodes[0][1].u();
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(nodes[1][0].v() - nodes[0][0].v() + dy*nodes[1][1].v() - nodes[0][1].v()));

    return 0.5*EA/L*std::pow(L_new - L, 2);
}
