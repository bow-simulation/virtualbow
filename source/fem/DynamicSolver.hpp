#pragma once
#include "fem/System.hpp"
#include "fem/Node.hpp"

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
