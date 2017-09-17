#include "ConstraintElement.hpp"

ConstraintElement::ConstraintElement(Node nd0, Node nd1, double k)
    : dofs{nd0[0], nd0[1], nd0[2], nd1[0], nd1[1]},
      k(k)
{
    double dx_abs = dofs[3].u() - dofs[0].u();
    double dy_abs = dofs[4].u() - dofs[1].u();

    dx_rel =  dx_abs*cos(dofs[2].u()) + dy_abs*sin(dofs[2].u());
    dy_rel = -dx_abs*sin(dofs[2].u()) + dy_abs*cos(dofs[2].u());
}

void ConstraintElement::update_state()
{
    c1 = dx_rel*cos(dofs[2].u()) - dy_rel*sin(dofs[2].u()) + dofs[0].u() - dofs[3].u();
    c2 = dx_rel*sin(dofs[2].u()) + dy_rel*cos(dofs[2].u()) + dofs[1].u() - dofs[4].u();
}

void ConstraintElement::get_masses(VectorView<Dof> M) const
{

}

void ConstraintElement::get_internal_forces(VectorView<Dof> q) const
{
    Vector<5> Dc1, Dc2;
    Dc1 << 1.0, 0.0, -dx_rel*sin(dofs[2].u()) - dy_rel*cos(dofs[2].u()), -1.0,  0.0;
    Dc2 << 0.0, 1.0,  dx_rel*cos(dofs[2].u()) - dy_rel*sin(dofs[2].u()),  0.0, -1.0;

    q(dofs) += k*(c1*Dc1 + c2*Dc2);
}

void ConstraintElement::get_tangent_stiffness(MatrixView<Dof> K) const
{
    // Todo: Code duplication
    Vector<5> Dc1, Dc2;
    Dc1 << 1.0, 0.0, -dx_rel*sin(dofs[2].u()) - dy_rel*cos(dofs[2].u()), -1.0,  0.0;
    Dc2 << 0.0, 1.0,  dx_rel*cos(dofs[2].u()) - dy_rel*sin(dofs[2].u()),  0.0, -1.0;

    // Todo: Don't actually create sparse matrices DDc1, DDc2
    Matrix<5> DDc1 = Matrix<5>::Zero();
    DDc1(2, 2) = dy_rel*sin(dofs[2].u()) - dx_rel*cos(dofs[2].u());

    Matrix<5> DDc2 = Matrix<5>::Zero();
    DDc2(2, 2) = -dy_rel*cos(dofs[2].u()) - dx_rel*sin(dofs[2].u());

    K(dofs) += k*(Dc1*Dc1.transpose() + Dc2*Dc2.transpose() + c1*DDc1 + c2*DDc2);
}

double ConstraintElement::get_potential_energy() const
{
    return 0.5*k*(c1*c1 + c2*c2);
}
