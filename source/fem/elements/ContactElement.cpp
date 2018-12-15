#include "ContactElement.hpp"
#include "numerics/Geometry.hpp"
#include "fem/System.hpp"

ContactForce::ContactForce(double k, double epsilon)
    : k(k), epsilon(epsilon)
{

}

double ContactForce::force(double e) const
{
    if(e <= 0.0)
    {
        return 0.0;
    }
    if(e <= epsilon)
    {
        return k/(2.0*epsilon)*e*e;
    }
    else
    {
        return k*(e-0.5*epsilon);
    }
}

double ContactForce::stiffness(double e) const
{
    if(e <= 0.0)
    {
        return 0.0;
    }
    if(e <= epsilon)
    {
        return k/epsilon*e;
    }
    else
    {
        return k;
    }
}

double ContactForce::energy(double e) const
{
    if(e <= 0.0)
    {
        return 0.0;
    }
    if(e <= epsilon)
    {
        return k/(6.0*epsilon)*e*e*e;
    }
    else
    {
        return 0.5*k*e*e - 0.5*k*epsilon*e + k/6.0*epsilon*epsilon;
    }
}

ContactElement::ContactElement(System& system, Node node0, Node node1, Node node2, double h0, double h1, ContactForce f)
    : Element(system),
      dofs{node0.x, node0.y, node0.phi, node1.x, node1.y, node1.phi, node2.x, node2.y},
      h0(h0), h1(h1), f(f)
{

}

void ContactElement::add_masses() const
{

}

void ContactElement::add_internal_forces() const
{
    State state = get_state();
    system.add_q(dofs, f.force(state.e)*state.De);
}

void ContactElement::add_tangent_stiffness() const
{
    State state = get_state();
    system.add_K(dofs, f.stiffness(state.e)*state.De*state.De.transpose() + f.force(state.e)*state.e*state.DDe);
}

double ContactElement::get_potential_energy() const
{
    State state = get_state();
    return f.energy(state.e);
}

double ContactElement::get_kinetic_energy() const
{
    return 0.0;
}

ContactElement::State ContactElement::get_state() const
{
    Vector<2> P0{system.get_u(dofs[0]), system.get_u(dofs[1])};
    Vector<2> P1{system.get_u(dofs[3]), system.get_u(dofs[4])};
    Vector<2> P2{system.get_u(dofs[6]), system.get_u(dofs[7])};
    Vector<2> Q0{system.get_u(dofs[0]) + h0*sin(system.get_u(dofs[2])),
                 system.get_u(dofs[1]) - h0*cos(system.get_u(dofs[2]))};
    Vector<2> Q1{system.get_u(dofs[3]) + h1*sin(system.get_u(dofs[5])),
                 system.get_u(dofs[4]) - h1*cos(system.get_u(dofs[5]))};

    // If no contact, set kinematic expressions to zero and return
    if(get_orientation(P2, P0, Q0) == Orientation::LeftHanded ||
       get_orientation(P2, P1, P0) == Orientation::LeftHanded ||
       get_orientation(P2, Q0, Q1) == Orientation::LeftHanded ||
       get_orientation(P2, Q1, P1) == Orientation::LeftHanded)
    {
        return {0.0, Vector<8>::Zero(), Matrix<8>::Zero()};
    }

    // Contact: Calculate kinematic expressions

    // 1. Penetration e

    double a1 = system.get_u(dofs[3]) - system.get_u(dofs[0]) - h0*sin(system.get_u(dofs[2])) + h1*sin(system.get_u(dofs[5]));
    double a2 = system.get_u(dofs[4]) - system.get_u(dofs[1]) + h0*cos(system.get_u(dofs[2])) - h1*cos(system.get_u(dofs[5]));
    double a3 = system.get_u(dofs[6]) - system.get_u(dofs[0]) - h0*sin(system.get_u(dofs[2]));
    double a4 = system.get_u(dofs[7]) - system.get_u(dofs[1]) + h0*cos(system.get_u(dofs[2]));

    double e = (a1*a4 - a2*a3)/hypot(a1, a2);

    // 2. First derivative of e

    Vector<8> Da1, Da2, Da3, Da4;
    Da1 << -1.0, 0.0, -h0*cos(system.get_u(dofs[2])), 1.0, 0.0, h1*cos(system.get_u(dofs[5])), 0.0, 0.0;
    Da2 << 0.0, -1.0, -h0*sin(system.get_u(dofs[2])), 0.0, 1.0, h1*sin(system.get_u(dofs[5])), 0.0, 0.0;
    Da3 << -1.0, 0.0, -h0*cos(system.get_u(dofs[2])), 0.0, 0.0, 0.0, 1.0, 0.0;
    Da4 << 0.0, -1.0, -h0*sin(system.get_u(dofs[2])), 0.0, 0.0, 0.0, 0.0, 1.0;

    double b1 = 1.0/hypot(a1, a2);
    double b2 = (a2*a3 - a1*a4)/pow(a1*a1 + a2*a2, 1.5);

    auto v1 = a4*Da1 - a3*Da2 - a2*Da3 + a1*Da4;
    auto v2 = a1*Da1 + a2*Da2;

    Vector<8> De = b1*v1 + b2*v2;

    // 3. Second derivative of e
    // Todo: Don't actually create sparse matrices DDa1 ... DDa4

    Matrix<8> DDa1 = Matrix<8>::Zero();
    DDa1(2, 2) = h0*sin(system.get_u(dofs[2]));
    DDa1(5, 5) = -h1*sin(system.get_u(dofs[5]));

    Matrix<8> DDa2 = Matrix<8>::Zero();
    DDa2(2, 2) = -h0*cos(system.get_u(dofs[2]));
    DDa2(5, 5) = h1*cos(system.get_u(dofs[5]));

    Matrix<8> DDa3 = Matrix<8>::Zero();
    DDa3(2, 2) = h0*sin(system.get_u(dofs[2]));

    Matrix<8> DDa4 = Matrix<8>::Zero();
    DDa4(2, 2) = -h0*cos(system.get_u(dofs[2]));

    auto Db1 = -(a1*Da1 + a2*Da2)/pow(a1*a1 + a2*a2, 1.5);

    auto Db2 = hypot(a1, a2)/(pow(a1, 6) + 3.0*pow(a1, 4)*pow(a2, 2) + 3*pow(a1, 2)*pow(a2, 4) + pow(a2, 6))
             * ((2.0*a1*a1*a4 - a2*a2*a4 - 3.0*a1*a2*a3)*Da1
              + (a1*a1*a3 - 2.0*a2*a2*a3 + 3.0*a1*a2*a4)*Da2
              + (a2*a2*a2 + a1*a1*a2)*Da3
              + (-a1*a1*a1 - a1*a2*a2)*Da4);

    auto Dv1 = Da4*Da1.transpose() - Da3*Da2.transpose()
             - Da2*Da3.transpose() + Da1*Da4.transpose()
             + a4*DDa1 - a3*DDa2 - a2*DDa3 + a1*DDa4;

    auto Dv2 = Da1*Da1.transpose() + Da2*Da2.transpose()
             + a1*DDa1 + a2*DDa2;

    Matrix<8> DDe = Db1*v1.transpose() + Db2*v2.transpose() + b1*Dv1 + b2*Dv2;

    return {e, De, DDe};
}
