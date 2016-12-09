#include "Element2.hpp"
#include "Node2.hpp"

double Element2::get_kinetic_energy(VectorView<Dof2> v) const
{
    double e_kin = 0.0;
    get_masses(VectorView<Dof2>(nullptr, nullptr, [&](Dof2 dof, double val)
    {
        e_kin += 0.5*val*std::pow(v(dof), 2);
    }));

    return e_kin;
}
