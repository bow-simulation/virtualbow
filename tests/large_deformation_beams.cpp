#include "../src/fem/System.hpp"
#include "../src/fem/elements/BeamElement.hpp"

#include <catch.hpp>
#include <vector>
#include <iostream>

// "Large deformation of a cantilever beam subjected to a vertical tip load" as described in [1]
// [1] On the correct representation of bending and axial deformation in the absolute nodal coordinate formulation with an elastic line approach
// Johannes Gerstmayr, Hans Irschik, Journal of Sound and Vibration 318 (2008) 461-487
TEST_CASE("Large deformation cantilever")
{
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
    std::vector<BeamElement> elements;

    // Create nodes
    for(unsigned i = 0; i < N+1; ++i)
    {
        bool active = (i != 0);
        nodes.push_back(system.create_node({{double(i)/double(N)*L, 0.0, 0.0}}, {{active, active, active}}));
    }

    // Create elements
    for(unsigned i = 0; i < N; ++i)
    {
        BeamElement element(nodes[i], nodes[i+1], 0.0, L/double(N));
        element.set_stiffness(E*A, E*I, 0.0);
        elements.push_back(element);
    }
    system.add_elements(elements);


    system.set_external_force(nodes[N].y, F0);
    system.solve_statics_lc();

    // Tip displacements
    double ux_num = L - system.get_u(nodes[N].x);
    double uy_num = system.get_u(nodes[N].y);

    // Reference displacements according to [1]
    double ux_ref = 0.5089228704;
    double uy_ref = 1.2083981311;

    double error_x = std::abs((ux_num - ux_ref)/ux_ref);
    double error_y = std::abs((uy_num - uy_ref)/uy_ref);

    // Todo: Why is the precision not better?
    REQUIRE(error_x < 1.1e-3);
    REQUIRE(error_y < 1.1e-3);
}


// Static test "Bending of a pre-curved beam into a full circle" from [1]
// [1] On the correct representation of bending and axial deformation in the absolute nodal coordinate formulation with an elastic line approach
// Johannes Gerstmayr, Hans Irschik. Journal of Sound and Vibration 318 (2008) 461-487
TEST_CASE("Large deformation circular beam")
{
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
        nodes.push_back(system.create_node({{R*std::sin(phi), R*(std::cos(phi) - 1.0), 0.0}}, {{active, active, active}}));
    }

    // Create elements
    for(unsigned i = 0; i < N; ++i)
    {
        double dist = system.get_distance(nodes[i], nodes[i+1]);
        double angle = system.get_angle(nodes[i], nodes[i+1]);

        BeamElement element(nodes[i], nodes[i+1], 0.0, dist);
        element.set_stiffness(EA, EI, 0.0);
        element.set_reference_angles(angle - system.get_u(nodes[i].phi),
                                     angle - system.get_u(nodes[i+1].phi));

        elements.push_back(element);
    }
    system.add_elements(elements);

    system.solve_statics_dc(nodes[N].phi, -M_PI, 15, [&]()
    {
        //std::cout << "phi = " << system.get_u()(nodes[N].phi) << "\n";
    });

    double M_num = -system.get_external_force(nodes[N].phi);
    double M_ref = EI*R;


    double error_M = std::abs((M_num - M_ref)/M_ref);
    double error_x = std::abs(system.get_u(nodes[N].x));
    double error_y = std::abs(system.get_u(nodes[N].y));

    // Todo: Why is the error in the torque so much higher than the displacement error?
    REQUIRE(error_M < 1.1e-3);
    REQUIRE(error_x < 1.0e-14);
    REQUIRE(error_y < 1.0e-14);
}

