#include "fem/System.hpp"
#include "fem/Solver.hpp"
#include "fem/elements/BarElement.hpp"
#include "fem/elements/MassElement.hpp"

#include <catch.hpp>
#include <iostream>

TEST_CASE("harmonic-oscillator")
{
    // https://de.wikipedia.org/wiki/Schwingung#Linear_ged.C3.A4mpfte_Schwingung
    double l = 1.0;
    double k = 100.0;
    double m = 5.0;

    double s0 = 0.1;   // Initial displacement

    System system;
    Node node_a = system.create_node({DofType::Fixed,  DofType::Fixed, DofType::Fixed}, {   0.0, 0.0, 0.0});
    Node node_b = system.create_node({DofType::Active, DofType::Fixed, DofType::Fixed}, {l + s0, 0.0, 0.0});

    system.add_element(BarElement(node_a, node_b, l, l*k, 0.0));
    system.add_element(MassElement(node_b, m, 0.0));

    // Constants for the analytical solution
    double omega = std::sqrt(k/m);                          // Natural frequency
    double T = 2.0*M_PI/omega;                              // Period length

    DynamicSolver solver(system, 0.01, 1e-15, [&]{ return system.t() >= T; });
    while(solver.step())
    {
        // Time
        double t = system.t();

        //std::cout << "t = " << t << "\n";

        // Numerical solution
        double s_num = node_b[0].u() - l;
        double v_num = node_b[0].v();

        // Analytical solution
        double s_ref = s0*cos(omega*t);
        double v_ref = -s0*omega*sin(omega*t);

        // Error
        double error_s = std::abs(s_num - s_ref);
        double error_v = std::abs(v_num - v_ref);

        REQUIRE(error_s < 0.62e-5);
        REQUIRE(error_v < 0.50e-4);
    }
}

