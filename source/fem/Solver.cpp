#include "Solver.hpp"

StaticSolverLC::StaticSolverLC(System& system): system(system)
{

}

void StaticSolverLC::find_equilibrium()
{
    double norm_0 = (system.get_p() - system.get_q()).norm();

    // With norm_0 = 0 the norm ratio criterion down below would be undefined,
    // so return here as the system is already in equilibrium anyway
    if(norm_0 == 0)
        return;

    for(unsigned i = 0; i < max_iter; ++i)
    {
        stiffness_dec.compute(system.get_K());
        if(stiffness_dec.info() != Eigen::Success)
        {
            throw std::runtime_error("Stiffness matrix decomposition failed");
        }

        system.set_u(system.get_u() + stiffness_dec.solve(system.get_p() - system.get_q()));
        double norm_i = (system.get_p() - system.get_q()).norm();

        if(norm_i/norm_0 < epsilon)
            return;

    }

    throw std::runtime_error("Maximum number of iterations exceeded");
}

StaticSolverDC::StaticSolverDC(System& system, Dof dof, double u_end, unsigned steps)
    : system(system),
      dof(dof),
      displacements(system.get_u()(dof.index), u_end, steps),
      e(unit_vector(system.dofs(), dof.index))
{

}

bool StaticSolverDC::step()
{
    auto u_next = displacements.next();

    if(u_next)
    {
        find_equilibrium(*u_next);
        return true;
    }

    return false;
}

void StaticSolverDC::find_equilibrium(double displacement)
{
    // Todo: Better control flow
    for(unsigned i = 0; i < max_iter; ++i)
    {
        delta = system.get_p() - system.get_q();

        stiffness_dec.compute(system.get_K());
        if(stiffness_dec.info() != Eigen::Success)
        {
            throw std::runtime_error("Decomposition of the stiffness matrix failed");
        }

        alpha = stiffness_dec.solve(delta);
        beta = stiffness_dec.solve(e);
        double df = (displacement - system.get_u()(dof.index) - alpha(dof.index))/beta(dof.index);

        system.set_u(system.get_u() + alpha + df*beta);
        system.set_p(dof, system.get_p(dof) + df);

        if((alpha + df*beta).norm()/double(system.dofs()) < epsilon)    // Todo: Better convergence criterion
        {
            return;
        }
    }

    throw std::runtime_error("Maximum number of iterations exceeded");
}

VectorXd StaticSolverDC::unit_vector(size_t n, size_t i) const
{
    assert(i < n);

    VectorXd vec = VectorXd::Zero(n);
    vec(i) = 1.0;

    return vec;
}

#include <iostream>

DynamicSolver::DynamicSolver(System& system, double step_factor, double sampling_rate, std::function<bool()> stop)
    : system(system),
      stop(stop),
      f(sampling_rate),
      t(0.0)
{
    // Timestep estimation
    Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd>
            eigen_solver(system.get_K(), system.get_M().asDiagonal(), Eigen::DecompositionOptions::EigenvaluesOnly);

    if(eigen_solver.info() != Eigen::Success)
        throw std::runtime_error("Failed to compute eigenvalues of the system");

    double omega_max = std::sqrt(eigen_solver.eigenvalues().maxCoeff());
    dt = (omega_max != 0.0) ? step_factor*2.0/omega_max : f/10.0;    // Todo: Magic number (Fraction of sampling time)

    // Initialise previous displacement
    u_p2 = system.get_u() - dt*system.get_v() + dt*dt/2.0*system.get_a();
}

bool DynamicSolver::step()
{
    while(system.get_t() < t)
    {
        sub_step();
        if(stop())
            return false;
    }

    t = system.get_t() + 1.0/f;
    return true;
}

void DynamicSolver::sub_step()
{
    u_p1 = system.get_u();

    system.set_u(2.0*system.get_u() - u_p2 + dt*dt*system.get_a());
    system.set_v((1.5*system.get_u() - 2.0*u_p1 + 0.5*u_p2)/dt);
    system.set_t(system.get_t() + dt);

    u_p2 = u_p1;
}
