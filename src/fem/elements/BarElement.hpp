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
    double L_dot;   // Time derivative of length

public:
    BarElement(Node nd0, Node nd1, double L, double EA, double etaA, double rhoA)
        : nodes{{nd0, nd1}},
          L(L),
          EA(EA),
          etaA(etaA),
          rhoA(rhoA)
    {

    }

    virtual void set_state(const VectorView<Dof> u, const VectorView<Dof> v)
    {
        dx = u(nodes[1].x) - u(nodes[0].x);
        dy = u(nodes[1].y) - u(nodes[0].y);

        L_new = std::hypot(dx, dy);  // Actual length
        L_dot = 1.0/L*(dx*(v(nodes[1].x) - v(nodes[0].x)) + dy*(v(nodes[1].y) - v(nodes[0].y)));
    }

    virtual void get_masses(VectorView<Dof> M) const override
    {
        double m = 0.5*rhoA*L;

        M(nodes[0].x) += m;
        M(nodes[0].y) += m;
        M(nodes[1].x) += m;
        M(nodes[1].y) += m;
    }

    virtual void get_internal_forces(VectorView<Dof> q) const override
    {
        double N = get_normal_force();

        q(nodes[0].x) -= N*dx/L;
        q(nodes[0].y) -= N*dy/L;
        q(nodes[1].x) += N*dx/L;
        q(nodes[1].y) += N*dy/L;
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

/*
pub struct BarElement
{
    nodes: [RcMut<Node2d>; 2],

    L: f64,     // Length
    rhoA: f64,
    etaA: f64,
    EA: f64,    // Longitudinial stiffness
}

impl BarElement
{
    pub fn new(node0: &RcMut<Node2d>, node1: &RcMut<Node2d>, L: f64, rhoA: f64, etaA: f64, EA: f64) -> BarElement
    {
        assert!(L != 0.0);  // Todo: Use more asserts when there are numbers with restricted domains

        BarElement
        {
            nodes: [node0.clone(), node1.clone()],

            L: L,
            rhoA: rhoA,
            etaA: etaA,
            EA: EA,
        }
    }

    pub fn get_length(&self) -> f64
    {
        self.L
    }

    pub fn set_length(&mut self, L: f64)
    {
        self.L = L;
    }
}

impl Element for BarElement
{
    fn mass_matrix(&mut self)
    {
        let m = 0.5*self.rhoA*self.L;

        self.nodes[0].dofs[0].m += m;
        self.nodes[0].dofs[1].m += m;
        self.nodes[1].dofs[0].m += m;
        self.nodes[1].dofs[1].m += m;
    }

    fn internal_forces(&mut self, K: Option<&mut MatrixView>)
    {
        let dx = self.nodes[1].dofs[0].u - self.nodes[0].dofs[0].u;
        let dy = self.nodes[1].dofs[1].u - self.nodes[0].dofs[1].u;

        let L = dx.hypot(dy);
        let dot_L = 1.0/L*(dx*(self.nodes[1].dofs[0].v - self.nodes[0].dofs[0].v) + dy*(self.nodes[1].dofs[1].v - self.nodes[0].dofs[1].v));
        let N = self.EA/self.L*(L - self.L) + self.etaA/self.L*dot_L;   // Axial force

        self.nodes[0].dofs[0].q -= N*dx/L;
        self.nodes[0].dofs[1].q -= N*dy/L;
        self.nodes[1].dofs[0].q += N*dx/L;
        self.nodes[1].dofs[1].q += N*dy/L;

        if let Some(K) = K
        {
            let c0 = self.EA*(L - self.L)/(L*self.L);   // Todo: Unify computation with axial force
            let c1 = self.EA/L.powi(3);

            // col 0
            K.add_sym(&self.nodes[0].dofs[0], &self.nodes[0].dofs[0], c1*dx*dx + c0);

            // col 1
            K.add_sym(&self.nodes[0].dofs[0], &self.nodes[0].dofs[1], c1*dx*dy);
            K.add_sym(&self.nodes[0].dofs[1], &self.nodes[0].dofs[1], c1*dy*dy + c0);

            // col 2
            K.add_sym(&self.nodes[0].dofs[0], &self.nodes[1].dofs[0], -c1*dx*dx - c0);
            K.add_sym(&self.nodes[0].dofs[1], &self.nodes[1].dofs[0], -c1*dx*dy);
            K.add_sym(&self.nodes[1].dofs[0], &self.nodes[1].dofs[0],  c1*dx*dx + c0);

            // col 3
            K.add_sym(&self.nodes[0].dofs[0], &self.nodes[1].dofs[1], -c1*dx*dy);
            K.add_sym(&self.nodes[0].dofs[1], &self.nodes[1].dofs[1], -c1*dy*dy - c0);
            K.add_sym(&self.nodes[1].dofs[0], &self.nodes[1].dofs[1],  c1*dx*dy);
            K.add_sym(&self.nodes[1].dofs[1], &self.nodes[1].dofs[1],  c1*dy*dy + c0);
        }
    }

    fn potential_energy(&self, V: &mut f64)
    {
        // Todo: Perhaps store these instead of recalculating or at least make a function to prevent code repetition
        let dx = self.nodes[1].dofs[0].u - self.nodes[0].dofs[0].u;
        let dy = self.nodes[1].dofs[1].u - self.nodes[0].dofs[1].u;
        let  L = dx.hypot(dy);

        *V += 0.5*self.EA/self.L*(L - self.L).powi(2);
    }
}
*/
