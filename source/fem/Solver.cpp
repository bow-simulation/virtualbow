#include "Solver.hpp"

StaticSolverLC::StaticSolverLC(System& system): system(system)
{

}

void StaticSolverLC::find_equilibrium()
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

StaticSolverDC::StaticSolverDC(System& system, Dof dof, double u_end, unsigned steps)
    : system(system),
      dof(dof),
      displacements(system.u()(dof.index()), u_end, steps),
      e(unit_vector(system.dofs(), dof.index()))
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
        delta = system.p() - system.q();

        stiffness_dec.compute(system.K());
        if(stiffness_dec.info() != Eigen::Success)
        {
            throw std::runtime_error("Decomposition of the stiffness matrix failed");
        }

        alpha = stiffness_dec.solve(delta);
        beta = stiffness_dec.solve(e);
        double df = (displacement - system.u()(dof.index()) - alpha(dof.index()))/beta(dof.index());

        system.u_mut() += alpha + df*beta;
        dof.p_mut() += df;

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
    Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd>
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
