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

void BarElement::get_masses(VectorView<Dof> M) const
{
    double m = 0.5*rhoA*L;

    M(nodes[0].x) += m;
    M(nodes[0].y) += m;
    M(nodes[1].x) += m;
    M(nodes[1].y) += m;
}

void BarElement::get_internal_forces(VectorView<Dof> q) const
{
    // Todo: Code duplication
    double dx = system.get_u(nodes[1].x) - system.get_u(nodes[0].x);
    double dy = system.get_u(nodes[1].y) - system.get_u(nodes[0].y);
    double L_new = std::hypot(dx, dy);
    double L_dot = 1.0/L_new*(dx*(system.get_v(nodes[1].x) - system.get_v(nodes[0].x))
                            + dy*(system.get_v(nodes[1].y) - system.get_v(nodes[0].y)));

    double N = get_normal_force();

    q(nodes[0].x) -= N*dx/L_new;
    q(nodes[0].y) -= N*dy/L_new;
    q(nodes[1].x) += N*dx/L_new;
    q(nodes[1].y) += N*dy/L_new;
}

void BarElement::get_tangent_stiffness(MatrixView<Dof> K) const
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
    K(nodes[0].x, nodes[0].x) += c1*dx*dx + c0;

    // col 1
    K(nodes[0].x, nodes[0].y) += c1*dx*dy;
    K(nodes[0].y, nodes[0].y) += c1*dy*dy + c0;

    // col 2
    K(nodes[0].x, nodes[1].x) += -c1*dx*dx - c0;
    K(nodes[0].y, nodes[1].x) += -c1*dx*dy;
    K(nodes[1].x, nodes[1].x) +=  c1*dx*dx + c0;

    // col 3
    K(nodes[0].x, nodes[1].y) += -c1*dx*dy;
    K(nodes[0].y, nodes[1].y) += -c1*dy*dy - c0;
    K(nodes[1].x, nodes[1].y) +=  c1*dx*dy;
    K(nodes[1].y, nodes[1].y) +=  c1*dy*dy + c0;
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
