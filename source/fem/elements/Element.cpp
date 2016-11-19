#include "Element.hpp"

void Element::set_state(const VectorView<Dof> u, const VectorView<Dof> v)
{
    this->u = u;
    this->v = v;
}

double Element::get_kinetic_energy() const
{
    double e_kin = 0.0;
    get_masses(VectorView<Dof>(nullptr, nullptr, [&](Dof dof, double val)
    {
        e_kin += 0.5*val*std::pow(v(dof), 2);
    }));

    return e_kin;
}

void Element::accumulate_energy(double& e_pot, double& e_kin) const
{
    e_pot += get_potential_energy();
    e_kin += get_kinetic_energy();
}
