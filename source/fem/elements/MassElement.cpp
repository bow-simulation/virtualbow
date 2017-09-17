#include "MassElement.hpp"

MassElement::MassElement(Node nd, double m, double I)
    : node(nd),
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

void MassElement::update_state()
{

}

void MassElement::get_masses(VectorView M) const
{
    M(node[0]) += m;
    M(node[1]) += m;
    M(node[2]) += I;
}

void MassElement::get_internal_forces(VectorView q) const
{

}

void MassElement::get_tangent_stiffness(MatrixView K) const
{

}

double MassElement::get_potential_energy() const
{
    return 0.0;
}
