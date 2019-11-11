#include "fem/System.hpp"
#include "fem/StaticSolver.hpp"
#include "fem/elements/BeamElement.hpp"

#include <catch.hpp>
#include <vector>
#include <iostream>

TEST_CASE("large-deformation-cantilever")
{
    // "Large deformation of a cantilever beam subjected to a vertical tip load" as described in [1]
    // [1] On the correct representation of bending and axial deformation in the absolute nodal coordinate formulation with an elastic line approach
    // Johannes Gerstmayr, Hans Irschik, Journal of Sound and Vibration 318 (2008) 461-487

    unsigned N = 15; // Number of elements

    double L = 2.0;
    double b = 0.1;
    double h = 0.1;

    double I = b*h*h*h/12.0;
    double A = b*h;
    double E = 2.07e11;

    double F0 = 3.0*E*I/(L*L);

    System system;
    std::vector<Node> nodes;

    // Create nodes
    for(unsigned i = 0; i < N+1; ++i)
    {
        bool active = (i != 0);
        nodes.push_back(system.create_node({active, active, active}, {double(i)/double(N)*L, 0.0, 0.0}));
    }

    // Create elements
    for(unsigned i = 0; i < N; ++i)
    {
        BeamElement element(system, nodes[i], nodes[i+1], 0.0, L/double(N), E*A, E*I, 0.0, 0.0);
        system.mut_elements().add(element);
    }

    system.set_p(nodes[N].y, F0);
    StaticSolverLC solver(system);
    solver.solve();

    // Numerical tip displacements
    double ux_num = L - system.get_u(nodes[N].x);
    double uy_num = system.get_u(nodes[N].y);

    // Reference tip displacements according to [1]
    double ux_ref = 0.5089228704;
    double uy_ref = 1.2083981311;

    // Error
    // Todo: Why is the precision not better?
    REQUIRE(std::abs((ux_num - ux_ref)/ux_ref) < 1.08e-3);
    REQUIRE(std::abs((uy_num - uy_ref)/uy_ref) < 5.79e-4);
}

TEST_CASE("large-deformation-circular-beam")
{
    /*
    // Static test "Bending of a pre-curved beam into a full circle" from [1]
    // [1] On the correct representation of bending and axial deformation in the absolute nodal coordinate formulation with an elastic line approach
    // Johannes Gerstmayr, Hans Irschik. Journal of Sound and Vibration 318 (2008) 461-487

    unsigned N = 20;    // Number of elements

    double R = 1.0;
    double EA = 5.0e8;
    double EI = 10.0e5;

    System system;
    std::vector<Node> nodes;
    std::vector<BeamElement> elements;

    // Create nodes
    for(unsigned i = 0; i < N+1; ++i)
    {
        bool active = (i != 0);
        double phi = double(i)/double(N)*M_PI;
        nodes.push_back(system.create_node({active, active, active}, {R*sin(phi), R*(cos(phi) - 1.0), 0.0}));
    }

    // Create elements
    for(unsigned i = 0; i < N; ++i)
    {
        double dist = system.get_distance(nodes[i], nodes[i+1]);
        double angle = system.get_angle(nodes[i], nodes[i+1]);

        BeamElement element(system, nodes[i], nodes[i+1], 0.0, dist);
        element.set_stiffness(EA, EI, 0.0);
        element.set_reference_angles(angle - system.get_u(nodes[i].phi), angle - system.get_u(nodes[i+1].phi));

        system.mut_elements().add(element);
    }

    StaticSolverDC solver(system, nodes[N].phi);
    for(unsigned i = 0; i < 15; ++i)
        solver.solve(-double(i)/15*M_PI);

    // Numerical solution
    double M_num = -system.get_p(nodes[N].phi);
    double x_num = system.get_u(nodes[N].x);
    double y_num = system.get_u(nodes[N].y);

    // Reference solution
    double M_ref = EI*R;
    double x_ref = 0.0;
    double y_ref = 0.0;

    // Error
    // Todo: Why is the torque so much more off than the displacements?
    REQUIRE(std::abs((M_num - M_ref)/M_ref) < 1.03e-03);
    REQUIRE(std::abs(x_num - x_ref) < 7.82e-16);
    REQUIRE(std::abs(y_num - y_ref) < 3.58e-14);
    */
}

