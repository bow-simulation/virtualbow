#include "EigenvalueSolver.hpp"
#include <algorithm>

ModeInfo::ModeInfo(std::complex<double> lambda)
{
    omega = std::hypot(lambda.real(), lambda.imag());
    zeta = -lambda.real()/omega;
}

EigenvalueSolver::EigenvalueSolver(const System& system)
    : system(system)
{

}

auto EigenvalueSolver::compute_eigenvalues()
{
    int n = system.dofs();
    A.conservativeResize(2*n, 2*n);
    B.conservativeResize(2*n, 2*n);

    A << MatrixXd::Zero(n, n), system.get_K(),
         system.get_K(), system.get_D();

    B << system.get_K(), MatrixXd::Zero(n, n),
         MatrixXd::Zero(n, n), -system.get_M().asDiagonal().toDenseMatrix();

    solver.compute(A, B, Eigen::DecompositionOptions::EigenvaluesOnly);
    if(solver.info() != Eigen::Success)
        throw std::runtime_error("Failed to compute eigenvalues of the system. Solver info: " + std::to_string(solver.info()));

    return solver.eigenvalues();
}

// Todo: Implement with filter + minimum
ModeInfo EigenvalueSolver::compute_minimum_frequency()
{
    std::optional<ModeInfo> result = std::nullopt;
    auto eigenvalues = compute_eigenvalues();
    for(int i = 0; i < eigenvalues.size(); ++i)
    {
        if(eigenvalues[i].imag() > 0.0)
        {
            ModeInfo temp(eigenvalues[i]);
            if(!result || temp.omega < result->omega)
            {
                result = temp;
            }
        }
    }

    if(!result)
        throw std::runtime_error("Failed to find eigenvalue with lowest natural frequency");

    return *result;
}

// Todo: Implement with filter + maximum
ModeInfo EigenvalueSolver::compute_maximum_frequency()
{
    std::optional<ModeInfo> result = std::nullopt;
    auto eigenvalues = compute_eigenvalues();
    for(int i = 0; i < eigenvalues.size(); ++i)
    {
        if(eigenvalues[i].imag() > 0.0)
        {
            ModeInfo temp(eigenvalues[i]);
            if(!result || temp.omega > result->omega)
            {
                result = temp;
            }
        }
    }

    if(!result)
        throw std::runtime_error("Failed to find eigenvalue with highest natural frequency");

    return *result;
}
