#include "BarElement.hpp"
#include "fem/System.hpp"

BarElement::BarElement(System& system, Node node0, Node node1, double L, double EA, double rhoA)
    : Element(system),
      dofs{node0.x, node0.y, node1.x, node1.y},
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
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    return EA/L*(L_new - L);
}

void BarElement::add_masses() const
{
    double m = 0.5*rhoA*L;
    system.add_M(dofs, Vector<4>{
        m,
        m,
        m,
        m
    });
}

void BarElement::add_internal_forces() const
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    double N = get_normal_force();

    system.add_q(dofs, Vector<4>{
        -N*dx/L_new,
        -N*dy/L_new,
         N*dx/L_new,
         N*dy/L_new
    });
}

void BarElement::add_tangent_stiffness() const
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    double c0 = EA*(L_new - L)/(L_new*L);
    double c1 = EA/pow(L_new, 3);

    Matrix<4> K;
    K << c1*dx*dx + c0,       c1*dx*dy, -c1*dx*dx - c0,      -c1*dx*dy,
              c1*dx*dy,  c1*dy*dy + c0,      -c1*dx*dy, -c1*dy*dy - c0,
        -c1*dx*dx - c0,      -c1*dx*dy,  c1*dx*dx + c0,       c1*dx*dy,
             -c1*dx*dy, -c1*dy*dy - c0,       c1*dx*dy,  c1*dy*dy + c0;

    system.add_K(dofs, K);
}

double BarElement::get_potential_energy() const
{
    // Todo: Code duplication
    double dx = system.get_u(dofs[2]) - system.get_u(dofs[0]);
    double dy = system.get_u(dofs[3]) - system.get_u(dofs[1]);
    double L_new = std::hypot(dx, dy);

    return 0.5*EA/L*std::pow(L_new - L, 2);
}

double BarElement::get_kinetic_energy() const
{
    return 0.25*rhoA*L*(pow(system.get_v(dofs[0]), 2)
                      + pow(system.get_v(dofs[1]), 2)
                      + pow(system.get_v(dofs[2]), 2)
                      + pow(system.get_v(dofs[3]), 2));
}
