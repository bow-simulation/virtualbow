#include "DynamicSolver.hpp"

DynamicSolver::DynamicSolver(System& system, double dt, double f, const StopFn& stop)
    : system(system),
      stop(stop),
      dt(dt),
      n(std::max(1.0/(f*dt), 1.0))
{
    // Initialise previous displacement
    u_p2 = system.get_u() - dt*system.get_v() + dt*dt/2.0*system.get_a();
}

// Estimate timestep based on maximum eigen frequency and a safety factor to account for nonlinearity of the system
double DynamicSolver::estimate_timestep(const System& system, double factor)
{
    Eigen::GeneralizedSelfAdjointEigenSolver<MatrixXd>
            eigen_solver(system.get_K(), system.get_M().asDiagonal(), Eigen::DecompositionOptions::EigenvaluesOnly);

    if(eigen_solver.info() != Eigen::Success)
        throw std::runtime_error("Failed to compute eigenvalues of the system");

    double omega_max = std::sqrt(eigen_solver.eigenvalues().maxCoeff());
    if(omega_max == 0.0)
        throw std::runtime_error("Can't estimate timestep for system with a zero eigenvalue");

    return factor*2.0/omega_max;
}

bool DynamicSolver::step()
{
    for(unsigned i = 0; i < n; ++i)
    {
        sub_step();
        if(stop())
            return false;
    }

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
