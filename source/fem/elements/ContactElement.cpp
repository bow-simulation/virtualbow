#include "ContactElement.hpp"

ContactElement::ContactElement(System& system, Node nd0, Node nd1, Node nd2, double h0, double h1, double k)
    : Element(system),
      nodes{nd0, nd1, nd2},
      h0(h0),
      h1(h1),
      k(k)
{

}

void ContactElement::add_masses() const
{

}

void ContactElement::add_internal_forces() const
{
    /*
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
    */
}

void ContactElement::add_tangent_stiffness() const
{
    /*
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
    */
}

double ContactElement::get_potential_energy() const
{
    /*
    // Todo: Code duplication
    double dx = nodes[1][0].u() - nodes[0][0].u();
    double dy = nodes[1][1].u() - nodes[0][1].u();
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(nodes[1][0].v() - nodes[0][0].v() + dy*nodes[1][1].v() - nodes[0][1].v()));

    return 0.5*EA/L*std::pow(L_new - L, 2);
    */

    return 0.0;
}

double ContactElement::get_kinetic_energy() const
{
    return 0.0;
}
