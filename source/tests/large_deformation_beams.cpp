#include "fem/System.hpp"
#include "fem/Solver.hpp"
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
        DofType type = (i == 0) ? DofType::Fixed : DofType::Active;
        nodes.push_back(system.create_node({type, type, type}, {double(i)/double(N)*L, 0.0, 0.0}));
    }

    // Create elements
    for(unsigned i = 0; i < N; ++i)
    {
        BeamElement element(nodes[i], nodes[i+1], 0.0, L/double(N));
        element.set_stiffness(E*A, E*I, 0.0);
        system.add_element(element);
    }

    nodes[N][1].p_mut() = F0;
    StaticSolverLC solver(system);
    solver.find_equilibrium();

    // Tip displacements
    double ux_num = L - nodes[N][0].u();
    double uy_num = nodes[N][1].u();

    // Reference displacements according to [1]
    double ux_ref = 0.5089228704;
    double uy_ref = 1.2083981311;

    double error_x = std::abs((ux_num - ux_ref)/ux_ref);
    double error_y = std::abs((uy_num - uy_ref)/uy_ref);

    // Todo: Why is the precision not better?
    REQUIRE(error_x < 1.08e-3);
    REQUIRE(error_y < 5.79e-4);
}

TEST_CASE("large-deformation-circular-beam")
{
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
        DofType type = (i == 0) ? DofType::Fixed : DofType::Active;
        double phi = double(i)/double(N)*M_PI;
        nodes.push_back(system.create_node({type, type, type}, {R*std::sin(phi), R*(std::cos(phi) - 1.0), 0.0}));
    }

    // Create elements
    for(unsigned i = 0; i < N; ++i)
    {
        double dist = Node::distance(nodes[i], nodes[i+1]);
        double angle = Node::angle(nodes[i], nodes[i+1]);

        BeamElement element(nodes[i], nodes[i+1], 0.0, dist);
        element.set_stiffness(EA, EI, 0.0);
        element.set_reference_angles(angle - nodes[i][2].u(), angle - nodes[i+1][2].u());

        system.add_element(element);
    }

    StaticSolverDC solver(system, nodes[N][2], -M_PI, 15);
    while(solver.step())
    {
        //std::cout << "phi = " << nodes[N][2].u() << "\n";
    }

    double M_num = -nodes[N][2].p();
    double M_ref = EI*R;

    double error_M = std::abs((M_num - M_ref)/M_ref);
    double error_x = std::abs(nodes[N][0].u());
    double error_y = std::abs(nodes[N][1].u());

    // Todo: Why is the error in the torque so much higher than the displacement error?
    REQUIRE(error_M < 1.03e-03);
    REQUIRE(error_x < 7.82e-16);
    REQUIRE(error_y < 3.58e-14);
}

