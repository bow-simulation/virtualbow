#include "Solver.hpp"

StaticSolverLC::StaticSolverLC(System& system): system(system)
{

}

#include <iostream>

/*
void StaticSolverLC::find_equilibrium()
{
    for(unsigned i = 0; i < max_iter; ++i)
    {
        stiffness_dec.compute(system.K());
        if(stiffness_dec.info() != Eigen::Success)
            throw std::runtime_error("Stiffness matrix decomposition failed");

        system.u_mut() += stiffness_dec.solve(system.p() - system.q());
        double norm_i = (system.p() - system.q()).norm();

        if(norm_i < epsilon)
            return;
    }

    throw std::runtime_error("LC Solver: Maximum number of iterations exceeded");
}
*/

#include "numerics/RootFinding.hpp"

//Full newton raphson with line search
void StaticSolverLC::find_equilibrium()
{
    for(unsigned int i = 0; i < max_iter; ++i)
    {
        stiffness_dec.compute(system.K());
        if(stiffness_dec.info() != Eigen::Success)
            throw std::runtime_error("Stiffness matrix decomposition failed");

        VectorXd ui = system.u();
        VectorXd fi = system.p() - system.q();
        VectorXd delta_u = stiffness_dec.solve(fi);

        //Line search
        ///////////////////////////////////////////////////////////

        auto s = [&](double alpha) -> double {
            system.u_mut() = ui + alpha*delta_u;
            VectorXd fi = system.p() - system.q();
            return delta_u.transpose()*fi;
        };

        //double alpha0 = 0.0;
        //double alpha1 = numerics::findSign<numerics::NEG>(s, 1.0, 1.5);
        //numerics::root(s, alpha0, alpha1, 0.0, 0.05);

        //for(double alpha = 0.0; alpha < 1.0; alpha += 0.01)
        //    std::cout << alpha << " " << s(alpha) << "\n";

        //std::cout << "=============================";

        //secant_method(s, 0.1, 0.5, 1e-3, 50);    // Todo: Magic numbers

        bracket_and_bisect<false>(s, 0.01, 1.5, 0.0, 0.01, 50);

        //bisect<false>(s, 0.05, 0.15, 0.0, 0.01, 500);

        ///////////////////////////////////////////////////////////

        ++i;

        double norm_i = (system.p() - system.q()).norm();
        if(norm_i < epsilon)
            return;
    }

    throw std::runtime_error("Model::staticEquilibrium() : No convergence to equilibrium state");
    //return false;
}

/*
//Full newton raphson with line search
bool Model::staticEquilibrium(double epsilon, unsigned int i_max)
{
    dot_u.setZero();

    for(unsigned int i = 0; i < i_max; ++i)
    {
        Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> stiffness_dec(tangentStiffness());

        if(stiffness_dec.info() != Eigen::Success)
        {
            throw std::runtime_error("Model::staticEquilibrium() : Stiffness matrix decomposition failed");
        }

        Eigen::VectorXd ui = u;
        Eigen::VectorXd fi = f_ext - internalForces();
        Eigen::VectorXd delta_u = stiffness_dec.solve(fi);

        //Line search
        ///////////////////////////////////////////////////////////

        auto s = [&](double alpha)
        {
            u = ui + alpha*delta_u;
            updateElements();
            Eigen::VectorXd fi = f_ext - internalForces();

            return delta_u.transpose()*fi;
        };

        double alpha0 = 0.0;
        double alpha1 = numerics::findSign<numerics::NEG>(s, 1.0, 1.5);
        numerics::root(s, alpha0, alpha1, 0.0, 0.05);

        ///////////////////////////////////////////////////////////

        ++i;

        if(delta_u.isZero(epsilon))
        {
            return true;
        }
    }

    throw std::runtime_error("Model::staticEquilibrium() : No convergence to equilibrium state");
    //return false;
}

*/




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
            throw std::runtime_error("Decomposition of the stiffness matrix failed");

        alpha = stiffness_dec.solve(delta);
        beta = stiffness_dec.solve(e);
        double df = (displacement - system.u()(dof.index()) - alpha(dof.index()))/beta(dof.index());

        system.u_mut() += alpha + df*beta;
        dof.p_mut() += df;

        if((alpha + df*beta).norm()/double(system.dofs()) < epsilon)    // Todo: Better convergence criterion
            return;
    }

    throw std::runtime_error("DC Solver: Maximum number of iterations exceeded");
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
