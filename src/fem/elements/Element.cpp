#include "Element.hpp"

double Element::get_kinetic_energy(const VectorView<Dof> v) const
{
    double T = 0.0;

    get_masses(VectorView<Dof>(nullptr, [&](Dof dof, double val)
    {
        T += 0.5*val*std::pow(v(dof), 2);
    }));

    return T;
}
