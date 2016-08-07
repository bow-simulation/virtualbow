#pragma once
#include "InputData.hpp"
#include "../numerics/StepFunction.hpp"

#include <boost/numeric/odeint.hpp>


struct DiscreteLimb
{
    std::vector<double> s;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> phi;

    std::vector<double> h;          // Total cross section height (used for contact)
    std::vector<double> Cee;
    std::vector<double> Ckk;
    std::vector<double> Cek;
    std::vector<double> rhoA;

    DiscreteLimb(const InputData& input)
    {
        calculate_nodes(input);
        calculate_properties(input);
    }

private:
    // Calculate positions and orientations of the nodes by integrating the limb curve from it's curvature.
    void calculate_nodes(const InputData& input)
    {
        using namespace boost::numeric::odeint;
        typedef std::array<double, 3> state_type;
        typedef runge_kutta_cash_karp54<state_type> error_stepper_type;

        StepFunction kappa(input.limb.curvature);
        auto system = [&](const state_type& z, state_type& dzds, double s)
        {
            dzds[0] = std::cos(z[2]);
            dzds[1] = std::sin(z[2]);
            dzds[2] = kappa(s);
        };

        auto observer = [&](const state_type &z, double s)
        {
            this->s.push_back(s);
            this->x.push_back(z[0]);
            this->y.push_back(z[1]);
            this->phi.push_back(z[2]);
        };

        state_type z0 = {input.limb.offset_x, input.limb.offset_y, input.limb.angle + M_PI_2};
        unsigned n = input.settings.n_elements_limb;
        double s0 = kappa.arg_min();
        double ds = (kappa.arg_max() - kappa.arg_min())/n;

        auto stepper = make_controlled<error_stepper_type>(1.0e-10, 1.0e-6);    // Todo: Magic numbers
        integrate_n_steps(stepper, system, z0, s0, ds, n, observer);
    }

    void calculate_properties(const InputData& /*input*/)
    {

    }
};
