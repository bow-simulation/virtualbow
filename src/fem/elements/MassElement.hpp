#pragma once
#include "Element.hpp"

#include <cmath>

class MassElement: public Element
{
private:
    Node node;

    double m;
    double I;

public:
    MassElement(Node nd, double m, double I)
        : node(nd), m(m), I(I)
    {

    }

    virtual void get_masses(VectorView<Dof> M) const override
    {
        M(node.x) += m;
        M(node.y) += m;
        M(node.phi) += m;
    }

    virtual void get_internal_forces(const VectorView<Dof> u, const VectorView<Dof> v, VectorView<Dof> q) const override
    {

    }

    virtual double get_potential_energy(const VectorView<Dof> u) const override
    {
        return 0.0;
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
