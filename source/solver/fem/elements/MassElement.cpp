#include "MassElement.hpp"
#include "solver/fem/System.hpp"

MassElement::MassElement(System& system, Node node, double m)
    : ElementBase(system, {node.x, node.y}),
      m(m)
{
    assert(m >= 0.0);
    assert(I >= 0.0);
}

double MassElement::get_mass() const
{
    return m;
}

void MassElement::set_mass(double m)
{
    this->m = m;
}

Vector<2> MassElement::get_mass_matrix() const
{
    return {m, m};
}

Matrix<2, 2> MassElement::get_tangent_stiffness_matrix(const Vector<2>& u) const
{
    return Matrix<2, 2>::Zero();
}

Matrix<2, 2> MassElement::get_tangent_damping_matrix(const Vector<2>& u) const
{
    return Matrix<2, 2>::Zero();
}

Vector<2> MassElement::get_internal_forces(const Vector<2>& u, const Vector<2>& v) const
{
    return Vector<2>::Zero();
}

double MassElement::get_potential_energy(const Vector<2>& u) const
{
    return 0.0;
}
