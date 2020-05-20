#pragma once
#include "solver/fem/ElementBase.hpp"

struct MassElementState {};
class MassElement: public ElementBase<2, MassElementState>
{
public:
    MassElement(System& system, Node node, double m);
    double get_mass() const;
    void set_mass(double m);

    virtual MassElementState compute_state(const Vector<2>& u, const Vector<2>& v) const override;
    virtual Vector<2> get_mass_matrix() const override;
    virtual Matrix<2> get_tangent_stiffness_matrix() const override;
    virtual Matrix<2> get_tangent_damping_matrix() const override;
    virtual Vector<2> get_internal_forces() const override;
    virtual double get_potential_energy() const override;

private:
    double m;
};
