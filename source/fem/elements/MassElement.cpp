#include "MassElement.hpp"
#include "fem/System.hpp"

MassElement::MassElement(System& system, Node node, double m, double I)
    : Element(system),
      dofs{node.x, node.y, node.phi},
      m(m),
      I(I)
{
    assert(m >= 0.0);
    assert(I >= 0.0);
}

void MassElement::set_node(Node node)
{
    dofs = {node.x, node.y, node.phi};
}

void MassElement::add_masses() const
{
    system.add_M(dofs, Vector<3>{m, m, I});
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
