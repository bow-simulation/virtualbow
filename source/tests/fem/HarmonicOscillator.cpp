#include "solver/fem/System.hpp"
#include "solver/fem/DynamicSolver.hpp"
#include "solver/fem/elements/BarElement.hpp"
#include "solver/fem/elements/MassElement.hpp"

#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("harmonic-oscillator")
{
    // https://de.wikipedia.org/wiki/Schwingung#Linear_ged.C3.A4mpfte_Schwingung
    double l = 1.0;
    double k = 100.0;
    double d = 10.0;
    double m = 5.0;

    double s0 = 0.1;   // Initial displacement

    System system;
    Node node_a = system.create_node({ false, false, false }, {    0.0, 0.0, 0.0 });
    Node node_b = system.create_node({ true, false, false },  { l + s0, 0.0, 0.0 });

    system.mut_elements().add(BarElement(system, node_a, node_b, l, l*k, l*d, 0.0));
    system.mut_elements().add(MassElement(system, node_b, m, 0.0));

    // Constants for the analytical solution
    double delta = d/(2.0*m);                                // Decay constant
    double omega0 = std::sqrt(k/m);                          // Natural frequency of the undamped system
    double omega = std::sqrt(omega0*omega0 - delta*delta);   // Natural frequency
    double T = 2.0*M_PI/omega;                               // Period length
    double A = delta/omega*s0;
    double B = s0;

    REQUIRE(delta < omega0);    // Make sure the system is underdamped

    DynamicSolver solver(system, 0.001, 1000.0, [&]{ return system.get_t() >= T; });
    while(solver.step())
    {
        // Numerical solution
        double s_num = system.get_u(node_b.x) - l;
        double v_num = system.get_v(node_b.x);

        // Analytical solution
        double t = system.get_t();
        double s_ref = std::exp(-delta*t)*(A*std::sin(omega*t) + B*std::cos(omega*t));
        double v_ref = -std::exp(-delta*t)*((omega*B + delta*A)*std::sin(omega*t) + (delta*B - omega*A)*std::cos(omega*t));

        // Error
        double error_s = std::abs(s_num - s_ref);
        double error_v = std::abs(v_num - v_ref);

        REQUIRE(error_s < 0.62e-5);
        REQUIRE(error_v < 0.50e-4);
    }
}
