#pragma once
#include "fem/System.hpp"
#include "fem/Node.hpp"
#include "numerics/Optimization.hpp"
#include <Eigen/Core>

class StaticSolver
{
public:
    // Todo: State for failed line search
    struct Info
    {
        enum {
            Success,
            DecompFailed,
            NoConvergence
        } outcome;
        unsigned iterations;
    };

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
    Info solve()
    {
        double lambda = 1.0;
        for(unsigned i = 0; i < max_iter; ++i)
        {
            decomp.compute(system.get_K());
            if(decomp.info() != Eigen::Success)
                return {Info::DecompFailed, i+1};

            delta_q = system.get_q() - lambda*system.get_p();
            alpha = -decomp.solve(delta_q);
            beta = decomp.solve(system.get_p());

            // Evaluate constraint
            constraint(system.get_u(), lambda, c, dcdl, dcdu);
            double delta_l;
            if((dcdl + dcdu.transpose()*beta) != 0)    // Todo: Epsilon
                delta_l = -(c + dcdu.transpose()*alpha)/(dcdl + dcdu.transpose()*beta);
            else
                delta_l = 0.0;

            delta_u = alpha + delta_l*beta;

            // Line search
            VectorXd u_start = system.get_u();
            double l_start = lambda;
            auto f = [&](double eta)
            {
                system.set_u(u_start + eta*delta_u);
                lambda = l_start + eta*delta_l;
                return std::abs(delta_u.transpose()*(system.get_q() - lambda*system.get_p()));
            };

            golden_section_search(f, 0.0, 1.0, 1e-2, 50);

            // If convergence...
            if(std::abs(delta_u.transpose()*delta_q) + std::abs(delta_l*c) < epsilon)    // Todo: Better convergence criterion
            {
                // ...apply load factor to the system and return
                system.set_p(lambda*system.get_p());
                return {Info::Success, i+1};
            }
        }

        return {Info::NoConvergence, max_iter};
    }

    virtual void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const = 0;

private:
    System& system;

    const unsigned max_iter = 150;    // Todo: Magic number
    const double epsilon = 1e-5;      // Todo: Magic number

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

    Info solve()
    {
        return StaticSolver::solve();
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

    Info solve(double displacement)
    {
        target = displacement;
        return StaticSolver::solve();
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
