#pragma once
#include "Element.hpp"

#include <cmath>

class BarElement: public Element
{
private:
    std::array<Node, 2> nodes;

    // Parameters
    double L;
    double EA;
    double etaA;
    double rhoA;

    // State
    double dx;
    double dy;
    double L_new;   // Actual length
    double L_dot;   // Time derivative of actual length

public:
    BarElement(Node nd0, Node nd1, double L, double EA, double etaA, double rhoA)
        : nodes{{nd0, nd1}},
          L(L),
          EA(EA),
          etaA(etaA),
          rhoA(rhoA)
    {

    }

    void set_length(double val)
    {
        L = val;
    }

    double get_node_mass() const    // Todo: Unify with beam element, use this function in mass matrix
    {
        return 0.5*rhoA*L;
    }

    virtual void set_state(const VectorView<Dof> u, const VectorView<Dof> v) override
    {
        dx = u(nodes[1].x) - u(nodes[0].x);
        dy = u(nodes[1].y) - u(nodes[0].y);

        L_new = std::hypot(dx, dy);
        L_dot = 1.0/L_new*(dx*(v(nodes[1].x) - v(nodes[0].x)) + dy*(v(nodes[1].y) - v(nodes[0].y)));

        Element::set_state(u, v);
    }

    virtual void get_masses(VectorView<Dof> M) const override
    {
        double m = get_node_mass();

        M(nodes[0].x) += m;
        M(nodes[0].y) += m;
        M(nodes[1].x) += m;
        M(nodes[1].y) += m;
    }

    virtual void get_internal_forces(VectorView<Dof> q) const override
    {
        double N = get_normal_force();

        q(nodes[0].x) -= N*dx/L_new;
        q(nodes[0].y) -= N*dy/L_new;
        q(nodes[1].x) += N*dx/L_new;
        q(nodes[1].y) += N*dy/L_new;
    }

    virtual void get_tangent_stiffness(MatrixView<Dof> K) const override
    {
        double c0 = EA*(L_new - L)/(L_new*L);
        double c1 = EA/std::pow(L_new, 3);

        // col 0
        K(nodes[0].x, nodes[0].x) += c1*dx*dx + c0;

        // col 1
        K(nodes[0].x, nodes[0].y) += c1*dx*dy;
        K(nodes[0].y, nodes[0].y) += c1*dy*dy + c0;

        // col 2
        K(nodes[0].x, nodes[1].x) += -c1*dx*dx - c0;
        K(nodes[0].y, nodes[1].x) += -c1*dx*dy;
        K(nodes[1].x, nodes[1].x) +=  c1*dx*dx + c0;

        // col 3
        K(nodes[0].x, nodes[1].y) += -c1*dx*dy;
        K(nodes[0].y, nodes[1].y) += -c1*dy*dy - c0;
        K(nodes[1].x, nodes[1].y) +=  c1*dx*dy;
        K(nodes[1].y, nodes[1].y) +=  c1*dy*dy + c0;
    }

    virtual double get_potential_energy() const override
    {
        return 0.5*EA/L*std::pow(L_new - L, 2);
    }

    double get_normal_force() const
    {
        return EA/L*(L_new - L) + etaA/L*L_dot;
    }
};
