#include "BarElement.hpp"
#include "solver/fem/System.hpp"

BarElement::BarElement(System& system, Node node0, Node node1, double L, double EA, double etaA, double rhoA)
    : Element(system),
      dofs{node0.x, node0.y, node1.x, node1.y},
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

void BarElement::set_length(double L)
{
    this->L = L;
}

void BarElement::set_stiffness(double EA)
{
    this->EA = EA;
}

void BarElement::set_damping(double etaA)
{
    this->etaA = etaA;
}

double BarElement::get_normal_force() const
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    double dvx = system.get_v(dofs[2]) - system.get_v(dofs[0]);
    double dvy = system.get_v(dofs[3]) - system.get_v(dofs[1]);
    double L_dot = (dx*dvx + dy*dvy)/L_new;

    return EA/L*(L_new - L) + etaA/L*L_dot;
}

void BarElement::add_masses() const
{
    double m = 0.5*rhoA*L;
    system.add_M(dofs, Vector<4>{ m, m, m, m });
}

void BarElement::add_internal_forces() const
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    double N = get_normal_force();
    system.add_q(dofs, N/L_new*Vector<4>{-dx, -dy, dx, dy});
}

void BarElement::add_tangent_stiffness() const
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    double c0 = EA*(L_new - L)/(L_new*L);
    double c1 = EA/(L_new*L_new*L_new);

    Matrix<4, 4> K;
    K << c1*dx*dx + c0,       c1*dx*dy, -c1*dx*dx - c0,      -c1*dx*dy,
              c1*dx*dy,  c1*dy*dy + c0,      -c1*dx*dy, -c1*dy*dy - c0,
        -c1*dx*dx - c0,      -c1*dx*dy,  c1*dx*dx + c0,       c1*dx*dy,
             -c1*dx*dy, -c1*dy*dy - c0,       c1*dx*dy,  c1*dy*dy + c0;

    system.add_K(dofs, K);
}

void BarElement::add_tangent_damping() const
{
    Matrix<4, 4> D;
    D << etaA/L,     0.0, -etaA/L,    0.0,
            0.0,  etaA/L,     0.0,-etaA/L,
        -etaA/L,     0.0,  etaA/L,    0.0,
            0.0, -etaA/L,     0.0, etaA/L;

    system.add_D(dofs, D);
}

double BarElement::get_potential_energy() const
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    return 0.5*EA/L*(L_new - L)*(L_new - L);
}

double BarElement::get_kinetic_energy() const
{
    Vector<4> v = system.get_v(dofs);
    return 0.25*rhoA*L*v.dot(v);
}
