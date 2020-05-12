#pragma once
#include "solver/fem/System.hpp"

// Represents a conjugate complex pair of eigenvalues
// lambda_1 = -zeta*omega + j*omega*sqrt(zeta^2 - 1)
// lambda_2 = -zeta*omega - j*omega*sqrt(zeta^2 - 1)
// Where omega is the undamped natural frequency and zeta is the damping ratio
struct ModeInfo
{
    double omega;    // Undamped natural frequency
    double zeta;       // Damping ratio

    ModeInfo(std::complex<double> lambda);
};

class EigenvalueSolver
{
public:
    EigenvalueSolver(const System& system);
    ModeInfo compute_minimum_frequency();
    ModeInfo compute_maximum_frequency();

private:
    const System& system;
    Eigen::GeneralizedEigenSolver<MatrixXd> solver;
    MatrixXd A;
    MatrixXd B;

    auto compute_eigenvalues();
};
