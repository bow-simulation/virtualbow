#pragma once
#include "fem/System.hpp"
#include "fem/Node.hpp"
#include <Eigen/Core>

#include <iostream>

class StaticSolver
{
public:
    StaticSolver(System& system)
        : system(system),
          delta_q(system.dofs()),
          delta_u(system.dofs()),
          alpha(system.dofs()),
          beta(system.dofs()),
          dcdu(system.dofs())
    {

    }

protected:
    void solve()
    {
        double lambda = 1.0;
        for(unsigned i = 0; i < max_iter; ++i)
        {
            decomp.compute(system.get_K());
            if(decomp.info() != Eigen::Success)
                throw std::runtime_error("Decomposition of the stiffness matrix failed");

            delta_q = system.get_q() - lambda*system.get_p();
            alpha = -decomp.solve(delta_q);
            beta = decomp.solve(system.get_p());

            // Evaluate constraint
            constraint(system.get_u(), lambda, c, dcdl, dcdu);
            double delta_l = -(c + dcdu.transpose()*alpha)/(dcdl + dcdu.transpose()*beta);
            delta_u = alpha + delta_l*beta;

            // Advance solution
            system.set_u(system.get_u() + delta_u);
            lambda += delta_l;

            // If convergence...
            if(std::abs(delta_u.transpose()*delta_q) + std::abs(delta_l*c) < epsilon)    // Todo: Better convergence criterion
            {
                // ...apply load factor to the system and return
                system.set_p(lambda*system.get_p());
                return;
            }
        }

        throw std::runtime_error("DC Solver: Maximum number of iterations exceeded");
    }

    virtual void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const = 0;

private:
    System& system;

    const unsigned max_iter = 50;         // Todo: Magic number
    const double epsilon = 1e-8;      // Todo: Magic number

    Eigen::LDLT<MatrixXd> decomp;
    VectorXd delta_q;
    VectorXd delta_u;
    VectorXd alpha;
    VectorXd beta;

    double c;
    double dcdl;
    VectorXd dcdu;
};

class StaticSolverLC: public StaticSolver
{
public:
    StaticSolverLC(System& system)
        : StaticSolver(system)
    {

    }

    void solve()
    {
        StaticSolver::solve();
    }

protected:
    virtual void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const override
    {
        c = lambda - 1.0;
        dcdl = 1.0;
        dcdu.setZero();
    }
};

class StaticSolverDC: public StaticSolver
{
public:
    StaticSolverDC(System& system, Dof dof)
        : StaticSolver(system),
          dof(dof),
          target(0.0),
          e_dof(unit_vector(system.dofs(), dof.index))
    {
        assert(dof.active);
    }

    void solve(double displacement)
    {
        target = displacement;
        StaticSolver::solve();
    }

protected:
    virtual void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const override
    {
        c = u(dof.index) - target;
        dcdl = 0.0;
        dcdu = e_dof;
    }

private:
    Dof dof;
    double target;
    VectorXd e_dof;
};
