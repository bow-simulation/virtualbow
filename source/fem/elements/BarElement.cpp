#include "BarElement.hpp"
#include "fem/System.hpp"

BarElement::BarElement(System& system, Node nd0, Node nd1, double L, double EA, double etaA, double rhoA)
    : Element(system),
      nodes{nd0, nd1},
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
    double dx = system.get_u(nodes[1].x) - system.get_u(nodes[0].x);
    double dy = system.get_u(nodes[1].y) - system.get_u(nodes[0].y);
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(system.get_v(nodes[1].x) - system.get_v(nodes[0].x))
                            + dy*(system.get_v(nodes[1].y) - system.get_v(nodes[0].y)));

    return EA/L*(L_new - L) + etaA/L*L_dot;
}

void BarElement::add_masses() const
{
    double m = 0.5*rhoA*L;
    system.add_M(nodes[0].x, m);
    system.add_M(nodes[0].y, m);
    system.add_M(nodes[1].x, m);
    system.add_M(nodes[1].y, m);
}

void BarElement::add_internal_forces() const
{
    // Todo: Code duplication
    double dx = system.get_u(nodes[1].x) - system.get_u(nodes[0].x);
    double dy = system.get_u(nodes[1].y) - system.get_u(nodes[0].y);
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(system.get_v(nodes[1].x) - system.get_v(nodes[0].x))
                            + dy*(system.get_v(nodes[1].y) - system.get_v(nodes[0].y)));

    double N = get_normal_force();

    system.add_q(nodes[0].x, -N*dx/L_new);
    system.add_q(nodes[0].y, -N*dy/L_new);
    system.add_q(nodes[1].x,  N*dx/L_new);
    system.add_q(nodes[1].y,  N*dy/L_new);
}

void BarElement::add_tangent_stiffness() const
{
    // Todo: Code duplication
    double dx = system.get_u(nodes[1].x) - system.get_u(nodes[0].x);
    double dy = system.get_u(nodes[1].y) - system.get_u(nodes[0].y);
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(system.get_v(nodes[1].x) - system.get_v(nodes[0].x))
                            + dy*(system.get_v(nodes[1].y) - system.get_v(nodes[0].y)));

    double c0 = EA*(L_new - L)/(L_new*L);
    double c1 = EA/std::pow(L_new, 3);

    // col 0
    system.add_K(nodes[0].x, nodes[0].x, c1*dx*dx + c0);

    // col 1
    system.add_K(nodes[0].x, nodes[0].y, c1*dx*dy);
    system.add_K(nodes[0].y, nodes[0].y, c1*dy*dy + c0);

    // col 2
    system.add_K(nodes[0].x, nodes[1].x, -c1*dx*dx - c0);
    system.add_K(nodes[0].y, nodes[1].x, -c1*dx*dy);
    system.add_K(nodes[1].x, nodes[1].x,  c1*dx*dx + c0);

    // col 3
    system.add_K(nodes[0].x, nodes[1].y, -c1*dx*dy);
    system.add_K(nodes[0].y, nodes[1].y, -c1*dy*dy - c0);
    system.add_K(nodes[1].x, nodes[1].y,  c1*dx*dy);
    system.add_K(nodes[1].y, nodes[1].y,  c1*dy*dy + c0);
}

double BarElement::get_potential_energy() const
{
    // Todo: Code duplication
    double dx = system.get_u(nodes[1].x) - system.get_u(nodes[0].x);
    double dy = system.get_u(nodes[1].y) - system.get_u(nodes[0].y);
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(system.get_v(nodes[1].x) - system.get_v(nodes[0].x))
                            + dy*(system.get_v(nodes[1].y) - system.get_v(nodes[0].y)));

    return 0.5*EA/L*std::pow(L_new - L, 2);
}

double BarElement::get_kinetic_energy() const
{
    return 0.0;    // Todo
}
