#include "DynamicSolver.hpp"

DynamicSolver::DynamicSolver(System& system, double end_time, double time_step, double sampling_rate)
    : system(system),
      stop([&]{ return system.get_t() >= end_time; }),
      f(sampling_rate),
      dt(time_step),
      t(0.0)
{
    // Initialise previous displacement
    // Todo: Code duplication
    u_p2 = system.get_u() - dt*system.get_v() + dt*dt/2.0*system.get_a();
}

#include <iostream>

DynamicSolver::DynamicSolver(System& system, double step_factor, double sampling_rate, std::function<bool()> stop)
    : system(system),
      stop(stop),
      f(sampling_rate),
      t(0.0)
{
    // Timestep estimation
    Eigen::GeneralizedSelfAdjointEigenSolver<MatrixXd>
            eigen_solver(system.get_K(), system.get_M().asDiagonal(), Eigen::DecompositionOptions::EigenvaluesOnly);

    if(eigen_solver.info() != Eigen::Success)
        throw std::runtime_error("Failed to compute eigenvalues of the system");

    double omega_max = std::sqrt(eigen_solver.eigenvalues().maxCoeff());

    if(omega_max == 0.0)
        throw std::runtime_error("Can't estimate timestep for system with zero eigenvalues");

    dt = step_factor*2.0/omega_max;    // Todo: Magic number (Fraction of sampling time)

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
