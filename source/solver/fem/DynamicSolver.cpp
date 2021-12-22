#include "DynamicSolver.hpp"
#include "System.hpp"
#include <Eigen/Eigenvalues>

DynamicSolver::DynamicSolver(System& system, double dt, double f_sample, const StopFn& stop)
    : system(system),
      stop(stop),
      dt(dt),
      n(std::max(std::ceil(1.0/(f_sample*dt)), 1.0))
{
    // Initialise previous displacement
    u_p2 = system.get_u() - dt*system.get_v() + dt*dt/2.0*system.get_a();
}

// Estimate timestep based on maximum eigen frequency and a safety factor to account for nonlinearity of the system
double DynamicSolver::estimate_timestep(const System& system, double factor) {
    /*
    // Version that includes damping
    // Problem: Calculation of eigenvalues is inefficient and not very robust
    // due to the matrices having 2 times the dimension compared to the version without damping below
    // https://www.dynasupport.com/tutorial/ls-dyna-users-guide/time-integration
    EigenvalueSolver solver(system);
    ModeInfo mode = solver.compute_maximum_frequency();
    return factor*2.0/mode.omega*(std::sqrt(1 + mode.zeta*mode.zeta) - mode.zeta);
    */

    // Ignore damping and use selfadjoint solver, which is more efficient and can handle larger matrices
    Eigen::GeneralizedSelfAdjointEigenSolver<MatrixXd>
            eigen_solver(system.get_K(), system.get_M().asDiagonal(), Eigen::DecompositionOptions::EigenvaluesOnly);

    if(eigen_solver.info() != Eigen::Success) {
        throw std::runtime_error("Failed to compute eigenvalues of the system");
    }

    double omega_max = std::sqrt(eigen_solver.eigenvalues().maxCoeff());
    if(omega_max == 0.0) {
        throw std::runtime_error("Can't estimate timestep for system with a zero eigenvalue");
    }

    return factor*2.0/omega_max;
}

bool DynamicSolver::step() {
    for(unsigned i = 0; i < n; ++i) {
        sub_step();
        if(stop()) {
            return false;
        }
    }

    return true;
}

void DynamicSolver::sub_step() {
    u_p1 = system.get_u();

    system.set_u(2.0*system.get_u() - u_p2 + dt*dt*system.get_a());
    system.set_v((1.5*system.get_u() - 2.0*u_p1 + 0.5*u_p2)/dt);
    system.set_t(system.get_t() + dt);

    u_p2 = u_p1;
}
