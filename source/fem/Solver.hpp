#pragma once
#include "System2.hpp"
#include "Node2.hpp"
#include "../numerics/Linspace.hpp"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <functional>

// Load controlled Newton-Raphson method
class StaticSolverLC
{
public:
    StaticSolverLC(System2& system): system(system)
    {

    }

    // Todo: Rename statics methods to 'solve_equilibrium' and 'solve_equilibrium_path'?
    void find_equilibrium()
    {
        double norm_0 = (system.p() - system.q()).norm();

        // With norm_0 = 0 the norm ratio criterion down below would be undefined,
        // so return here as the system is already in equilibrium anyway
        if(norm_0 == 0)
            return;

        for(unsigned i = 0; i < max_iter; ++i)
        {
            stiffness_dec.compute(system.K());
            if(stiffness_dec.info() != Eigen::Success)
            {
                throw std::runtime_error("Stiffness matrix decomposition failed");
            }

            system.u_mut() += stiffness_dec.solve(system.p() - system.q());
            double norm_i = (system.p() - system.q()).norm();

            if(norm_i/norm_0 < epsilon)
                return;

        }

        throw std::runtime_error("Maximum number of iterations exceeded");
    }

private:
    System2& system;

    const unsigned max_iter = 50;     // Todo: Magic number
    const double epsilon = 1e-6;      // Todo: Magic number

    Eigen::LDLT<Eigen::MatrixXd> stiffness_dec;
};

// Displacement controlled Newton-Raphson method
class StaticSolverDC
{
public:
    StaticSolverDC(System2& system, Dof2 dof, double u_end, unsigned steps)
        : system(system),
          dof(dof)
    {
        if(dof.type == DofType::Fixed)
            throw std::runtime_error("Displacement control not possible for fixed DOF");

        displacements = Linspace(system.u()(dof.index), u_end, steps);
    }

    bool step()
    {
        auto u_next = displacements.next();

        if(u_next)
        {
            find_equilibrium(*u_next);
            return true;
        }

        return false;
    }

private:
    System2& system;
    Dof2 dof;
    Linspace displacements;

    const unsigned max_iter = 50;     // Todo: Magic number
    const double epsilon = 1e-6;      // Todo: Magic number

    Eigen::LDLT<Eigen::MatrixXd> stiffness_dec;
    VectorXd delta = VectorXd(system.dofs());
    VectorXd alpha = VectorXd(system.dofs());
    VectorXd beta = VectorXd(system.dofs());
    VectorXd e = unit_vector(system.dofs(), dof.index);

    void find_equilibrium(double displacement)
    {
        // Todo: Better control flow

        for(unsigned i = 0; i < max_iter; ++i)
        {
            delta = system.p() - system.q();

            stiffness_dec.compute(system.K());
            if(stiffness_dec.info() != Eigen::Success)
            {
                throw std::runtime_error("Decomposition of the stiffness matrix failed");
            }

            alpha = stiffness_dec.solve(delta);
            beta = stiffness_dec.solve(e);
            double df = (displacement - system.u()(dof.index) - alpha(dof.index))/beta(dof.index);

            system.u_mut() += alpha + df*beta;
            system.p_mut()(dof.index) += df;

            if((alpha + df*beta).norm()/double(system.dofs()) < epsilon)    // Todo: Better convergence criterion
            {
                return;
            }
        }

        throw std::runtime_error("Maximum number of iterations exceeded");
    }

    static VectorXd unit_vector(size_t n, size_t i)
    {
        assert(i < n);

        VectorXd vec(n);
        vec(i) = 1.0;

        return vec;
    }
};

// Central difference method
class DynamicSolver
{
public:
    DynamicSolver(System2& system, double step_factor, double sampling_time, std::function<bool()> stopping_criterion)
        : system(system),
          stopping_criterion(stopping_criterion),
          stop(false),
          ts(sampling_time),
          t(0.0)
    {
        // Timestep estimation
        Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(system.K(), system.M().asDiagonal(), Eigen::DecompositionOptions::EigenvaluesOnly);
        if(eigen_solver.info() != Eigen::Success)
            throw std::runtime_error("Failed to compute eigenvalues of the system");

        double omega_max = std::sqrt(eigen_solver.eigenvalues().maxCoeff());
        dt = step_factor*2.0/omega_max;

        // Initialise previous displacement
        u_p2 = system.u() - dt*system.v() + dt*dt/2.0*system.a();
    }

    bool step()
    {
        if(stop)
            return false;

        while(system.t() < t)
        {
            stop = stop || !sub_step();  // Set stop to true when sub_step returns false (stopping criterion reached)
        }

        t = system.t() + ts;
        return true;
    }

private:
    System2& system;
    std::function<bool()> stopping_criterion;

    double dt;    // Timestep

    bool stop;    // Stopping criterion reached, continue until next sampling point
    double ts;    // Sampling time
    double t;     // Time at/after which the system needs to be sampled next

    VectorXd u_p2;
    VectorXd u_p1;

    bool sub_step()
    {
        u_p1 = system.u();

        system.u_mut() = 2.0*system.u() - u_p2 + dt*dt*system.a();
        system.v_mut() = (1.5*system.u() - 2.0*u_p1 + 0.5*u_p2)/dt;
        system.t_mut() += dt;

        u_p2 = u_p1;

        return !stopping_criterion();
    }
};
