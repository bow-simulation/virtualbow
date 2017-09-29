#pragma once
#include "fem/System.hpp"
#include "fem/Node.hpp"
#include "numerics/Linspace.hpp"

#include <Eigen/Core>

#include <functional>

// Load controlled Newton-Raphson method
class StaticSolverLC
{
public:
    StaticSolverLC(System& system);
    void find_equilibrium();

private:
    System& system;
    Eigen::LDLT<Eigen::MatrixXd> stiffness_dec;

    const unsigned max_iter = 50;         // Todo: Magic number
    const double epsilon_rel = 1e-4;      // Todo: Magic number
    const double epsilon_abs = 1e-6;      // Todo: Magic number
};

// Displacement controlled Newton-Raphson method
class StaticSolverDC
{
public:
    StaticSolverDC(System& system, Dof dof, double u_end, unsigned steps);
    bool step();

private:
    System& system;
    Dof dof;
    Linspace<double> displacements;

    const unsigned max_iter = 50;     // Todo: Magic number
    const double epsilon = 1e-6;      // Todo: Magic number

    Eigen::LDLT<MatrixXd> stiffness_dec;
    VectorXd delta;
    VectorXd alpha;
    VectorXd beta;
    VectorXd e;

    void find_equilibrium(double displacement);
    VectorXd unit_vector(size_t n, size_t i) const;
};

// Central difference method
class DynamicSolver
{
public:
    DynamicSolver(System& system, double end_time, double time_step, double sampling_rate);
    DynamicSolver(System& system, double step_factor, double sampling_time, std::function<bool()> stop);
    bool step();

private:
    System& system;
    std::function<bool()> stop;

    double dt;    // Timestep
    double f;     // Sampling rate
    double t;     // Time at which the system has to be sampled next

    VectorXd u_p2;
    VectorXd u_p1;

    void sub_step();
};
