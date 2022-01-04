#include "StaticSolver.hpp"
#include "solver/fem/System.hpp"
#include "solver/numerics/Optimization.hpp"

StaticSolver::StaticSolver(System& system)
    : system(system),
      delta_q(system.dofs()),
      delta_u(system.dofs()),
      alpha(system.dofs()),
      beta(system.dofs()),
      dcdu(system.dofs())
{

}

StaticSolver::Info StaticSolver::solve() {
    double lambda = 1.0;
    for(unsigned i = 0; i < max_iter; ++i) {
        decomp.compute(system.get_K());
        if(decomp.info() != Eigen::Success)
            return {Info::DecompFailed, i+1};

        delta_q = system.get_q() - lambda*system.get_p();
        alpha = -decomp.solve(delta_q);
        beta = decomp.solve(system.get_p());

        // Evaluate constraint
        constraint(system.get_u(), lambda, c, dcdl, dcdu);
        double delta_l;
        if((dcdl + dcdu.transpose()*beta) != 0) {    // Todo: Epsilon
            delta_l = -(c + dcdu.transpose()*alpha)/(dcdl + dcdu.transpose()*beta);
        }
        else {
            delta_l = 0.0;
        }

        delta_u = alpha + delta_l*beta;

        // Line search
        VectorXd u_start = system.get_u();
        double l_start = lambda;
        auto f = [&](double eta) {
            system.set_u(u_start + eta*delta_u);
            lambda = l_start + eta*delta_l;
            return std::abs(delta_u.transpose()*(system.get_q() - lambda*system.get_p()));
        };

        golden_section_search(f, 0.0, 1.0, 1e-2, 50);

        // If convergence...
        if(std::abs(delta_u.transpose()*delta_q) + std::abs(delta_l*c) < epsilon) {    // Todo: Better convergence criterion
            // ...apply load factor to the system and return
            system.set_p(lambda*system.get_p());
            return {Info::Success, i+1};
        }
    }

    return {Info::NoConvergence, max_iter};
}

StaticSolverLC::StaticSolverLC(System& system)
    : StaticSolver(system)
{

}

StaticSolver::Info StaticSolverLC::solve() {
    return StaticSolver::solve();
}

void StaticSolverLC::constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const {
    c = lambda - 1.0;
    dcdl = 1.0;
    dcdu.setZero();
}

StaticSolverDC::StaticSolverDC(System& system, Dof dof)
    : StaticSolver(system),
      dof(dof),
      target(0.0),
      e_dof(VectorXd::Unit(system.dofs(), dof.index))
{
    if(!dof.active) {
        throw std::invalid_argument("Displacement control not possible on fixed DOF");
    }
}

StaticSolver::Info StaticSolverDC::solve(double displacement) {
    target = displacement;
    return StaticSolver::solve();
}

void StaticSolverDC::constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const {
    c = u(dof.index) - target;
    dcdl = 0.0;
    dcdu = e_dof;
}
