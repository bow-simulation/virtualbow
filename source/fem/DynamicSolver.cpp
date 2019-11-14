#include "DynamicSolver.hpp"

using namespace boost::numeric::odeint;

DynamicSolver::DynamicSolver(System& system, double dt, double f_sample, const StopFn& stop)
    : system(system),
      stop_fn(stop),
      dt(dt),
      n(std::max(std::ceil(1.0/(f_sample*dt)), 1.0)),
      stepper(make_controlled(1e-6, 1e-3, runge_kutta_cash_karp54<state_type>())),
      x(2*system.dofs())
{
    // Assign system state to x
    Eigen::Map<VectorXd> u(&x[0], system.dofs());
    Eigen::Map<VectorXd> v(&x[system.dofs()], system.dofs());
    u = system.get_u();
    v = system.get_v();
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
        if(stop_fn())
            return false;
    }

    return true;
}

void DynamicSolver::sub_step()
{
    auto f = [&](const state_type& x, state_type& dxdt, const double t)
    {
        // Assign state and time to the system
        Eigen::Map<const VectorXd> u(&x[0], system.dofs());
        Eigen::Map<const VectorXd> v(&x[system.dofs()], system.dofs());
        system.set_u(u);
        system.set_v(v);
        system.set_t(t);

        // Assign system velocity and acceleration to dxdt
        Eigen::Map<VectorXd> dxdt_1(&dxdt[0], system.dofs());
        Eigen::Map<VectorXd> dxdt_2(&dxdt[system.dofs()], system.dofs());
        dxdt_1 = system.get_v();
        dxdt_2 = system.get_a();
    };

    //auto test = boost::numeric::odeint::make_controlled(1e-6, 1e-6, runge_kutta_cash_karp54<state_type>());

    double t = system.get_t();
    while(stepper.try_step(f, x, t, dt) != controlled_step_result::success)
    {

    }
}
