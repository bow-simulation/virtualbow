#include "MassElement.hpp"

MassElement::MassElement(Node nd, double m, double I)
    : node(nd),
      m(m),
      I(I)
{

}

void MassElement::set_node(Node nd)
{
    node = nd;
}

void MassElement::get_masses(VectorView<Dof> M) const
{
    M(node[0]) += m;
    M(node[1]) += m;
    M(node[2]) += I;
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
