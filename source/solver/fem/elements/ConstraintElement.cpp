#include "ConstraintElement.hpp"
#include "solver/fem/System.hpp"

ConstraintElement::ConstraintElement(System& system, Node node0, Node node1, double k)
    : Element(system),
      dofs{node0.x, node0.y, node0.phi, node1.x, node1.y},
      k(k)
{
    double dx_abs = system.get_u(dofs[3]) - system.get_u(dofs[0]);
    double dy_abs = system.get_u(dofs[4]) - system.get_u(dofs[1]);

    dx_rel =  dx_abs*cos(system.get_u(dofs[2])) + dy_abs*sin(system.get_u(dofs[2]));
    dy_rel = -dx_abs*sin(system.get_u(dofs[2])) + dy_abs*cos(system.get_u(dofs[2]));
}


void ConstraintElement::add_masses() const
{

}

void ConstraintElement::add_internal_forces() const
{
    double c1 = dx_rel*cos(system.get_u(dofs[2])) - dy_rel*sin(system.get_u(dofs[2]))
              + system.get_u(dofs[0]) - system.get_u(dofs[3]);
    double c2 = dx_rel*sin(system.get_u(dofs[2])) + dy_rel*cos(system.get_u(dofs[2]))
              + system.get_u(dofs[1]) - system.get_u(dofs[4]);

    Vector<5> Dc1, Dc2;
    Dc1 << 1.0, 0.0, -dx_rel*sin(system.get_u(dofs[2])) - dy_rel*cos(system.get_u(dofs[2])), -1.0,  0.0;
    Dc2 << 0.0, 1.0,  dx_rel*cos(system.get_u(dofs[2])) - dy_rel*sin(system.get_u(dofs[2])),  0.0, -1.0;

    system.add_q(dofs, k*(c1*Dc1 + c2*Dc2));
}

void ConstraintElement::add_tangent_stiffness() const
{
    double c1 = dx_rel*cos(system.get_u(dofs[2])) - dy_rel*sin(system.get_u(dofs[2]))
              + system.get_u(dofs[0]) - system.get_u(dofs[3]);
    double c2 = dx_rel*sin(system.get_u(dofs[2])) + dy_rel*cos(system.get_u(dofs[2]))
              + system.get_u(dofs[1]) - system.get_u(dofs[4]);

    // Todo: Code duplication
    Vector<5> Dc1, Dc2;
    Dc1 << 1.0, 0.0, -dx_rel*sin(system.get_u(dofs[2])) - dy_rel*cos(system.get_u(dofs[2])), -1.0,  0.0;
    Dc2 << 0.0, 1.0,  dx_rel*cos(system.get_u(dofs[2])) - dy_rel*sin(system.get_u(dofs[2])),  0.0, -1.0;

    // Todo: Don't actually create sparse matrices DDc1, DDc2
    Matrix<5, 5> DDc1 = Matrix<5, 5>::Zero();
    DDc1(2, 2) = dy_rel*sin(system.get_u(dofs[2])) - dx_rel*cos(system.get_u(dofs[2]));

    Matrix<5, 5> DDc2 = Matrix<5, 5>::Zero();
    DDc2(2, 2) = -dy_rel*cos(system.get_u(dofs[2])) - dx_rel*sin(system.get_u(dofs[2]));

    system.add_K(dofs, k*(Dc1*Dc1.transpose() + Dc2*Dc2.transpose() + c1*DDc1 + c2*DDc2));
}

void ConstraintElement::add_tangent_damping() const
{

}

double ConstraintElement::get_potential_energy() const
{
    double c1 = dx_rel*cos(system.get_u(dofs[2])) - dy_rel*sin(system.get_u(dofs[2]))
              + system.get_u(dofs[0]) - system.get_u(dofs[3]);
    double c2 = dx_rel*sin(system.get_u(dofs[2])) + dy_rel*cos(system.get_u(dofs[2]))
              + system.get_u(dofs[1]) - system.get_u(dofs[4]);

    return 0.5*k*(c1*c1 + c2*c2);
}

double ConstraintElement::get_kinetic_energy() const
{
    return 0.0;
}
