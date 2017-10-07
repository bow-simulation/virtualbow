#include "DynamicSolver.hpp"

DynamicSolver::DynamicSolver(System& system, double end_time, double time_step, double sampling_rate)
    : system(system),
      stop([&]{ return system.t() >= end_time; }),
      f(sampling_rate),
      dt(time_step),
      t(0.0)
{
    // Initialise previous displacement
    // Todo: Code duplication
    u_p2 = system.u() - dt*system.v() + dt*dt/2.0*system.a();
}

DynamicSolver::DynamicSolver(System& system, double step_factor, double sampling_rate, std::function<bool()> stop)
    : system(system),
      stop(stop),
      f(sampling_rate),
      t(0.0)
{
    // Timestep estimation
    Eigen::GeneralizedSelfAdjointEigenSolver<MatrixXd>
            eigen_solver(system.K(), system.M().asDiagonal(), Eigen::DecompositionOptions::EigenvaluesOnly);

    if(eigen_solver.info() != Eigen::Success)
        throw std::runtime_error("Failed to compute eigenvalues of the system");

    double omega_max = std::sqrt(eigen_solver.eigenvalues().maxCoeff());

    if(omega_max == 0.0)
        throw std::runtime_error("Can't estimate timestep for system with zero eigenvalues");

    dt = step_factor*2.0/omega_max;    // Todo: Magic number (Fraction of sampling time)

    // Initialise previous displacement
    u_p2 = system.u() - dt*system.v() + dt*dt/2.0*system.a();
}

bool DynamicSolver::step()
{
    while(system.t() < t)
    {
        sub_step();
        if(stop())
            return false;
    }

    t = system.t() + 1.0/f;
    return true;
}

void DynamicSolver::sub_step()
{
    u_p1 = system.u();

    system.u_mut() = 2.0*system.u() - u_p2 + dt*dt*system.a();
    system.v_mut() = (1.5*system.u() - 2.0*u_p1 + 0.5*u_p2)/dt;
    system.t_mut() += dt;

    u_p2 = u_p1;
}
