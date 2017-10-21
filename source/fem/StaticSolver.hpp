#pragma once
#include "fem/System.hpp"
#include "fem/Node.hpp"
#include "numerics/RootFinding.hpp"
#include <Eigen/Core>

#include <iostream>

template<class F>
double golden_section_search(const F& f, double xa, double xb, double xtol, unsigned iter)
{
    double gr = (sqrt(5) + 1)/2;

    double xc = xb - (xb - xa)/gr;
    double xd = xa + (xb - xa)/gr;

    for(unsigned i = 0; i < iter; ++i)
    {
        if(std::abs(xc - xd) < xtol)
            return (xb + xa)/2;

        if(f(xc) < f(xd))
            xb = xd;
        else
            xa = xc;

        xc = xb - (xb - xa)/gr;
        xd = xa + (xb - xa)/gr;
    }

    throw std::runtime_error("Golden section search: Maximum number of iterations exceeded");
}


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
            double delta_l;
            if((dcdl + dcdu.transpose()*beta) != 0)    // Todo: Epsilon
                delta_l = -(c + dcdu.transpose()*alpha)/(dcdl + dcdu.transpose()*beta);
            else
                delta_l = 0.0;

            delta_u = alpha + delta_l*beta;

            //std::cout << "==================== delta l  ===================\n";
            //std::cout << (dcdl + dcdu.transpose()*beta);


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

            /*
            try
            {
                //std::cout << bracket_and_bisect(f, 0.1, 1.2, 1e-3, 1e-2) << "\n";
                std::cout << golden_section_search(f, 0.0, 1.0, 1e-2, 50) << "\n";
            }
            catch(...)
            {

                exit(0);
            }
            */

            //std::cout << "====================" << golden_section_search(f, 0.0, 1.0, 1e-2, 50) << "\n";
            //for(double eta = 0.0; eta <= 1.0; eta += 0.01)
            //    std::cout << eta << "," << f(eta) << "\n";


            //exit(0);

            /*
            // Step size reduction
            double eta = 1.0;
            double delta_u_max = delta_u.maxCoeff();
            if(delta_u_max > 0.04)
            {
                eta = 0.04/delta_u_max;

                std::cout << "Step reduction, u_max = " << delta_u_max << ", eta = " << eta << "\n";
            }
            */

            // Advance solution
            //system.set_u(system.get_u() + delta_u);
            //lambda += delta_l;

            //std::cout << "error = " << std::abs(delta_u.transpose()*delta_q) + std::abs(delta_l*c) << "\n";

            // If convergence...
            if(std::abs(delta_u.transpose()*delta_q) + std::abs(delta_l*c) < epsilon)    // Todo: Better convergence criterion
            {
                // ...apply load factor to the system and return
                system.set_p(lambda*system.get_p());
                return;
            }
        }

        throw std::runtime_error("Static solver: Maximum number of iterations exceeded");
    }

    virtual void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const = 0;

private:
    System& system;

    const unsigned max_iter = 150;    // Todo: Magic number
    const double epsilon = 1e-6;      // Todo: Magic number

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
