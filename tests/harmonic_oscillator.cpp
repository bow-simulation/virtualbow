#include "../src/fem/System.hpp"
#include "../src/fem/elements/BarElement.hpp"
#include "../src/fem/elements/MassElement.hpp"

#include <catch.hpp>
#include <iostream>

TEST_CASE("Dynamic solution harmonic oscillator")
{
    // https://de.wikipedia.org/wiki/Schwingung#Linear_ged.C3.A4mpfte_Schwingung
    double l = 1.0;
    double k = 100.0;
    double d = 0.0*10.0;
    double m = 5.0;

    double s0 = 0.1;   // Initial displacement

    System system;
    Node node_a = system.create_node({{   0.0, 0.0, 0.0}}, {{false, false, false}});
    Node node_b = system.create_node({{l + s0, 0.0, 0.0}}, {{true, false, false}});

    BarElement element1(node_a, node_b, l, l*k, l*d, 0.0);
    MassElement element2(node_b, m, 0.0);
    system.add_element(element1);
    system.add_element(element2);

    // Constants for the analytical solution
    double delta = d/(2.0*m);                                // Decay constant
    double omega0 = std::sqrt(k/m);                          // Natural frequency of the undamped system
    double omega = std::sqrt(omega0*omega0 - delta*delta);   // Natural frequency
    double T = 2.0*M_PI/omega;                               // Period length
    double A = delta/omega*s0;
    double B = s0;

    //assert!(delta < omega0);                      // Make sure the system is underdamped  // Todo: Reintroduce this check

    system.solve_dynamics(0.01, [&]()
    {
        // Time
        double t = system.get_time();

        // Numerical solution
        double s_num = system.get_u()(node_b.x) - l;
        double v_num = system.get_v()(node_b.x);

        // Analytical solution
        double s_ref = std::exp(-delta*t)*(A*std::sin(omega*t) + B*std::cos(omega*t));
        double v_ref = -std::exp(-delta*t)*((omega*B + delta*A)*std::sin(omega*t) + (delta*B - omega*A)*std::cos(omega*t));

        // Error
        double error_s = std::abs(s_num - s_ref);
        double error_v = std::abs(v_num - v_ref);

        REQUIRE(error_s < 1e-4);
        REQUIRE(error_v < 1e-4);

        //std::cout << t << ", " << s_ref << "\n";
        //std::cout << error_s << "\n";

        return t < T;
    });
}

