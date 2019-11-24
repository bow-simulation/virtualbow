#pragma once
#include "fem/System.hpp"

class EigenvalueSolver
{
public:
    // Represents a conjugate complex pair of eigenvalues
    // lambda_1 = -zeta*omega0 + j*omega_0*sqrt(zeta^2 - 1)
    // lambda_2 = -zeta*omega0 - j*omega_0*sqrt(zeta^2 - 1)
    // Where omega_0 is the undamped natural frequency and zeta is the damping ratio
    struct ModeInfo
    {
        double omega_0;    // Undamped natural frequency
        double zeta;       // Damping ratio
    };

    EigenvalueSolver(const System& system);
    std::vector<ModeInfo> compute();

private:
    const System& system;

    Eigen::GeneralizedEigenSolver<MatrixXd> eigen_solver;
    MatrixXd A;
    MatrixXd B;
};
