#pragma once
#include "solver/fem/System.hpp"
#include "solver/fem/Node.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Utils.hpp"
#include "solver/numerics/RootFinding.hpp"
#include <iostream>

class StaticSolver
{
public:
    struct Settings
    {
        unsigned iter_max = 150;
        unsigned iter_ref = 5;
        double epsilon_rel = 1e-4;
        double epsilon_abs = 1e-12;
        bool full_update = true;
        bool line_search = false;
    };

    struct Outcome
    {
        unsigned iterations;
        bool success;
    };

    StaticSolver(System& system, Dof dof, Settings settings)
        : system(system), dof(dof), settings(settings)
    {

    }

    Outcome solve_equilibrium(double u_target)
    {
        backup_system_state();

        double lambda = 0.0;
        p0 = system.get_p();
        ek = unit_vector(system.dofs(), dof.index);

        double energy_g_0;
        for(unsigned i = 0; i < settings.iter_max; ++i)
        {
            // Calculate factorization of the system's tangent stiffness matrix
            // Always in the first iteration, in subsequent ones only for the full Newton-Raphson method
            if(i == 0 || settings.full_update) {
                decomp.compute(system.get_K());
                if(decomp.info() != Eigen::Success)
                {
                    restore_system_state();
                    return Outcome { .iterations = i, .success = false };
                }
            }

            // Calculate out of balance loads and auxiliary vectors alpha and beta
            delta_q = system.get_q() - system.get_p();
            alpha = -decomp.solve(delta_q);
            beta = decomp.solve(ek);

            // Evaluate constraint and calculate change in load parameter and displacement
            double c = system.get_u(dof) - u_target;
            double delta_l = -(c + ek.transpose()*alpha)/(ek.transpose()*beta);
            delta_u = alpha + delta_l*beta;

            // Calculate energies
            double energy_g_i = delta_u.transpose()*delta_q + delta_l*c;
            if(i == 0) {
                energy_g_0 = energy_g_i;
            }

            // Check for convergence
            if(std::abs(energy_g_0) < settings.epsilon_abs || energy_g_i/energy_g_0 < settings.epsilon_rel)
            {
                // Apply changes
                lambda = lambda + delta_l;
                system.set_u(system.get_u() + delta_u);
                system.set_p(p0 + lambda*ek);

                return Outcome { .iterations = i + 1, .success = true };
            }

            // Perform line search

            ui = system.get_u();
            double lambda_i = lambda;
            auto g = [&](double s)
            {
                lambda = lambda_i + s*delta_l;
                system.set_u(ui + s*delta_u);
                system.set_p(p0 + lambda*ek);

                delta_q = system.get_q() - system.get_p();
                double c = system.get_u(dof) - u_target;

                return delta_u.transpose()*delta_q + delta_l*c;
            };

            double g0 = g(0.0);
            double g1 = g(1.0);
            if(g0*g1 < 0.0)
            {
                double s = RootFinding::regula_falsi(g, 0.0, 1.0, g0, g1, 1e-10, 1e-3, 150);
                std::cout << "s = " << s << "\n";
            }

            //std::cout << "g(0) = " << g(0.0) << "\n";
            //std::cout << "g(1) = " << g(1.0) << "\n";
        }

        restore_system_state();
        return Outcome { .iterations = settings.iter_max, .success = false };
    }

    template<class F>
    void solve_equilibrium_path(double u_target, unsigned n_steps, const F& callback)
    {
        double delta_u_max = (u_target - system.get_u(dof))/double(n_steps);    // Maximum step size such that the total number of steps is >= n_steps
        double delta_u_min = delta_u_max/100.0;

        double delta_u = delta_u_max;
        double u_last = system.get_u(dof);    // Last successful step (excep initially)
        double u_test = system.get_u(dof);    // Next step to try, may be unsuccessful

        while(u_last != u_target)
        {
            Outcome outcome = solve_equilibrium(u_test);
            if(outcome.success)
            {
                // Notify caller
                callback();

                // Remember successfull displacement
                u_last = u_test;

                // Adapt step size and cap at minnimum and maximum
                // Todo: sqrt or not?
                delta_u = clamp(delta_u*sqrt(double(settings.iter_ref)/outcome.iterations),
                                        delta_u_min, delta_u_max);

                // Apply step to last successful displacement, cap result at target displacement
                u_test = u_last + delta_u;
                if(sgn(u_target - u_test) != sgn(delta_u)) {
                    u_test = u_target;
                }
            }
            else
            {
                // Abort if step size is already equal to the minimum:
                // No convergence + step size can't be decreased anymore
                if(delta_u == delta_u_min) {
                    throw std::runtime_error("Can't decrease adaptive step size beyond lower limit");
                }

                // Decrease step size by constant factor, cap at minimum and maximum values
                delta_u = clamp(0.5*delta_u, delta_u_min, delta_u_max);
            }
        }
    }

private:
    void backup_system_state()
    {
        u_backup = system.get_u();
        p_backup = system.get_p();
    }

    void restore_system_state()
    {
        system.set_u(u_backup);
        system.set_p(p_backup);
    }

private:
    System& system;
    Dof dof;

    Settings settings;
    Eigen::LDLT<MatrixXd> decomp;
    VectorXd delta_q;
    VectorXd delta_u;
    VectorXd alpha;
    VectorXd beta;

    VectorXd ui;
    VectorXd p0;
    VectorXd ek;

    // State backup
    VectorXd u_backup;
    VectorXd p_backup;
};
