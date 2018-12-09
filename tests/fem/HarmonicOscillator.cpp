#include "fem/System.hpp"
#include "fem/DynamicSolver.hpp"
#include "fem/elements/BarElement.hpp"
#include "fem/elements/MassElement.hpp"

#include <catch.hpp>
#include <iostream>

TEST_CASE("harmonic-oscillator")
{
    // https://en.wikipedia.org/wiki/Harmonic_oscillator
    double l = 1.0;
    double k = 100.0;
    double m = 5.0;

    double s0 = 0.1;   // Initial displacement

    System system;
    Node node_a = system.create_node({false,  false, false}, {   0.0, 0.0, 0.0});
    Node node_b = system.create_node({true, false, false}, {l + s0, 0.0, 0.0});

    system.mut_elements().add(BarElement(system, node_a, node_b, l, l*k, 0.0));
    system.mut_elements().add(MassElement(system, node_b, m, 0.0));

    // Constants for the analytical solution
    double omega = std::sqrt(k/m);                          // Natural frequency
    double T = 2.0*M_PI/omega;                              // Period length

    DynamicSolver solver(system, 0.001, 1000.0, [&]{ return system.get_t() >= T; });
    while(solver.step())
    {
        // Numerical solution
        double s_num = system.get_u(node_b.x) - l;
        double v_num = system.get_v(node_b.x);

        // Analytical solution
        double t = system.get_t();
        double s_ref = s0*cos(omega*t);
        double v_ref = -s0*omega*sin(omega*t);

        // Error
        REQUIRE(std::abs(s_num - s_ref) < 0.62e-5);
        REQUIRE(std::abs(v_num - v_ref) < 0.50e-4);
    }
}

