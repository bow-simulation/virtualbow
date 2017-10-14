#include "MassElement.hpp"

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

void MassElement::get_masses(VectorView<Dof> M) const
{
    M(node.x) += m;
    M(node.y) += m;
    M(node.phi) += I;
}

void MassElement::get_internal_forces(VectorView<Dof> q) const
{

}

void MassElement::get_tangent_stiffness(MatrixView<Dof> K) const
{

}

double MassElement::get_potential_energy() const
{
    return 0.0;
}
