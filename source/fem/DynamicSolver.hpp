#pragma once
#include "fem/System.hpp"
#include "fem/Node.hpp"

#include <boost/numeric/odeint.hpp>
#include <vector>

using state_type = std::vector<double>;
using boost::numeric::odeint::runge_kutta_cash_karp54;
using stepper_type = boost::numeric::odeint::result_of::make_controlled<runge_kutta_cash_karp54<state_type>>::type;

// Central difference method
class DynamicSolver
{
public:
    using StopFn = std::function<bool()>;

    DynamicSolver(System& system, double dt, double f_sample, const StopFn& stop_fn);
    static double estimate_timestep(const System& system, double factor);
    bool step();

private:
    System& system;
    StopFn stop_fn;
    double dt;     // Delta for sub steps
    unsigned n;    // Number of substeps per step

    stepper_type stepper;
    state_type x;

    void sub_step();
    void get_system_state(state_type& state);
    void set_system_state(const state_type& state);
};
