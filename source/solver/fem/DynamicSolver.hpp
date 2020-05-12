#pragma once
#include "solver/fem/System.hpp"
#include "solver/fem/Node.hpp"
#include "EigenvalueSolver.hpp"

// Central difference method
class DynamicSolver
{
public:
    using StopFn = std::function<bool()>;

    DynamicSolver(System& system, double dt, double f_sample, const StopFn& stop);
    static double estimate_timestep(const System& system, double factor);
    bool step();

private:
    System& system;
    StopFn stop;

    double dt;     // Substep size
    unsigned n;    // Number of substeps per step

    VectorXd u_p2;
    VectorXd u_p1;

    void sub_step();
};
