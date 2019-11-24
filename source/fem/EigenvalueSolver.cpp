#include "EigenvalueSolver.hpp"
#include <algorithm>

EigenvalueSolver::EigenvalueSolver(const System& system)
    : system(system)
{

}

std::vector<EigenvalueSolver::ModeInfo> EigenvalueSolver::compute()
{
    int n = system.dofs();

    A.conservativeResize(2*n, 2*n);
    A << MatrixXd::Zero(n, n), MatrixXd::Identity(n, n),
         -system.get_K(), -system.get_D();

    B.conservativeResize(2*n, 2*n);
    B << MatrixXd::Identity(n, n), MatrixXd::Zero(n, n),
         MatrixXd::Zero(n, n), system.get_M().asDiagonal().toDenseMatrix();

    eigen_solver.compute(A, B, Eigen::DecompositionOptions::EigenvaluesOnly);
    if(eigen_solver.info() != Eigen::Success)
        throw std::runtime_error("Failed to compute eigenvalues of the system. Solver info: " + std::to_string(eigen_solver.info()));

    std::vector<ModeInfo> results;
    auto evs = eigen_solver.eigenvalues();
    for(int i = 0; i < evs.size(); ++i)
    {
        if(std::abs(evs(i).imag()) < 1e-12)    // Magic number
        {
            // Real eigenvalue: Skip
            continue;
        }
        else
        {
            // Complex eigenvalue
            double omega_0 = std::hypot(evs[i].real(), evs[i].imag());
            double zeta = -evs[i].real()/omega_0;
            results.push_back({omega_0, zeta});

            // Skip next conjugated eigenvalue
            ++i;
        }
    }

    // Sort by undamped eigenfrequency
    std::sort(results.begin(), results.end(), [](const ModeInfo& a, const ModeInfo& b) {
        return a.omega_0 < b.omega_0;
    });

    return results;
}

