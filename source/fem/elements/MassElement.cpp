#include "MassElement.hpp"
#include "fem/System.hpp"

MassElement::MassElement(System& system, Node nd, double m, double I)
    : Element(system),
      node(nd),
      m(m),
      I(I)
{
    assert(m >= 0.0);
    assert(I >= 0.0);
}

void MassElement::set_node(Node nd)
{
    node = nd;
}

void MassElement::add_masses() const
{
    system.add_M(node.x,   m);
    system.add_M(node.y,   m);
    system.add_M(node.phi, I);
}

void MassElement::add_internal_forces() const
{

}

void MassElement::add_tangent_stiffness() const
{

}

double MassElement::get_potential_energy() const
{
    return 0.0;
}

double MassElement::get_kinetic_energy() const
{
    return 0.0;    // Todo
}
