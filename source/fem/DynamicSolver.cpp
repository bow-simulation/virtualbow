#include "DynamicSolver.hpp"

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
double DynamicSolver::estimate_timestep(const System& system, double factor)
{
    EigenvalueSolver solver(system);

    //https://www.dynasupport.com/tutorial/ls-dyna-users-guide/time-integration
    ModeInfo mode = solver.compute_maximum_frequency();
    return factor*2.0/mode.omega*(std::sqrt(1 + mode.zeta*mode.zeta) - mode.zeta);
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
