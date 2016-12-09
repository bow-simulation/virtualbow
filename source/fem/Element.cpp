#include "Element.hpp"
#include "Node.hpp"

double Element::get_kinetic_energy(VectorView<Dof> v) const
{
    double e_kin = 0.0;
    get_masses(VectorView<Dof>(nullptr, nullptr, [&](Dof dof, double val)
    {
        e_kin += 0.5*val*std::pow(v(dof), 2);
    }));

    return e_kin;
}
