#include "fem/System.hpp"
#include "fem/StaticSolver.hpp"
#include "fem/elements/BeamElement.hpp"
#include "bow/BeamUtils.hpp"
#include "numerics/Linspace.hpp"

#include <catch.hpp>
#include <vector>
#include <iostream>
/*
TEST_CASE("large-deformation-cantilever")
{
    // "Large deformation of a cantilever beam subjected to a vertical tip load" as described in [1]
    // [1] On the correct representation of bending and axial deformation in the absolute nodal coordinate formulation with an elastic line approach
    // Johannes Gerstmayr, Hans Irschik, Journal of Sound and Vibration 318 (2008) 461-487

    unsigned N = 15;    // Number of elements

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
        Matrix<6, 6> K = BeamUtils::stiffness_matrix(E*A, E*I, L/double(N), 0.0);
        BeamElement element(system, nodes[i], nodes[i+1], K, Vector<6>::Zero());
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
    REQUIRE(std::abs((ux_num - ux_ref)/ux_ref) < 1e-3);
    REQUIRE(std::abs((uy_num - uy_ref)/uy_ref) < 1e-3);
}
*/
/*
TEST_CASE("large-deformation-circular-beam")
{
    // Static test "Bending of a pre-curved beam into a full circle" from [1]
    // [1] On the correct representation of bending and axial deformation in the absolute nodal coordinate formulation with an elastic line approach
    // Johannes Gerstmayr, Hans Irschik. Journal of Sound and Vibration 318 (2008) 461-487

    unsigned N = 10;    // Number of elements

    double R = 1.0;
    double L = M_PI*R;
    double EA = 1.0e6;
    double EI = 1.0e3;

    System system;
    std::vector<Node> nodes;

    auto r_fn = [&](double s) {
        return (Vector<3>() << R*sin(s/R), R*cos(s/R) - R, -s/R).finished();
    };

    auto C_fn = [&](double s) {
        return (Matrix<2, 2>() << EA, 0.0, 0.0, EI).finished();
    };

    // Create nodes
    std::cout << "Shape:\n";
    std::vector<double> s = Linspace<double>(0.0, L, N+1).collect();
    for(unsigned i = 0; i < s.size(); ++i)
    {
        bool active = (i != 0);
        Vector<3> r = r_fn(s[i]);
        std::cout << r[0] << ", " << r[1] << ", " << r[2] << "\n";

        nodes.push_back(system.create_node({active, active, active}, {r[0], r[1], r[2]}));
    }

    //return;

    // Create elements
    for(unsigned i = 0; i < N; ++i)
    {
        //Matrix<6, 6> K = BeamUtils::stiffness_matrix(EA, EI, system.get_distance(nodes[i], nodes[i+1]),
        //        system.get_angle(nodes[i], nodes[i+1]));

        Matrix<6, 6> K = BeamUtils::stiffness_matrix(r_fn, C_fn, s[i], s[i+1]);
        BeamElement element(system, nodes[i], nodes[i+1], K, Vector<6>::Zero());
        system.mut_elements().add(element);
    }


    StaticSolverLC solver(system);

    std::cout << "Trajectory:\n";
    unsigned steps = 100;
    for(double M: Linspace<double>(0.0, EI*R, steps))
    {
        system.set_p(nodes.back().phi, -M);
        solver.solve();

        std::cout << system.get_u(nodes.back().x) << ", " << system.get_u(nodes.back().y) << "\n";
    }

    // Numerical solution
    double x_num = system.get_u(nodes.back().x);
    double y_num = system.get_u(nodes.back().y);
    double phi_num = system.get_u(nodes.back().phi);

    // Reference solution
    double x_ref = 0.0;
    double y_ref = 0.0;
    double phi_ref = -2*M_PI;

    std::cout << "\n\n";
    std::cout << "phi_num = " << phi_num << "\n";
    std::cout << "x_num = " << x_num << "\n";
    std::cout << "y_num = " << y_num << "\n";

    return;

    // Error
    REQUIRE(std::abs(x_num - x_ref) < 1e-6);
    REQUIRE(std::abs(y_num - y_ref) < 1e-6);
    REQUIRE(std::abs(phi_num - phi_ref) < 1e-6);
}
*/
