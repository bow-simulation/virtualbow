#include "../fem/System.hpp"
#include "../fem/elements/BarElement.hpp"
#include "../fem/elements/BeamElement.hpp"

#include <catch.hpp>
#include <iostream>

void check_stiffness_matrix(System& system)
{
    Eigen::MatrixXd K_sym = Eigen::MatrixXd::Zero(system.dofs(), system.dofs());
    Eigen::MatrixXd K_num = Eigen::MatrixXd::Zero(system.dofs(), system.dofs());

    system.get_tangent_stiffness(K_sym);
    system.get_numeric_tangent_stiffness(K_num, 1e-8);

    REQUIRE(K_sym.isApprox(K_num, 1e-5));                   // Equal to numeric derivatives
    REQUIRE(K_sym.isApprox(K_num.transpose(), 1e-5));       // Symmetric
}

TEST_CASE("Tangent stiffness matrix: BarElement")
{
    auto check_at = [](double dx0, double dy0, double dx1, double dy1)
    {
        double L = 1.0;
        double EA = 100.0;

        System system;
        Node node0 = system.create_node({{true, true, false}}, {{    dx0, dy0, 0.0}});
        Node node1 = system.create_node({{true, true, false}}, {{L + dx1, dy1, 0.0}});

        BarElement element01(node0, node1, L, EA, 0.0, 0.0);
        system.add_element(element01);

        check_stiffness_matrix(system);
    };

    check_at(0.0, 0.0, 0.0, 0.0);

    check_at(0.0, 0.0,  1.5,  1.5);
    check_at(0.0, 0.0, -1.5,  1.5);
    check_at(0.0, 0.0,  1.5, -1.5);
    check_at(0.0, 0.0, -1.5, -1.5);

    check_at( 1.5,  1.5, 0.0, 0.0);
    check_at(-1.5,  1.5, 0.0, 0.0);
    check_at( 1.5, -1.5, 0.0, 0.0);
    check_at(-1.5, -1.5, 0.0, 0.0);
}

TEST_CASE("Tangent stiffness matrix: BeamElement")
{
    auto check_at = [](double x0, double y0, double phi0, double x1, double y1, double phi1)
    {
        double L = 1.0;
        double EA = 100.0;
        double EI = 10.0;

        System system;
        Node node0 = system.create_node({{true, true, true}}, {{x0, y0, phi0}});
        Node node1 = system.create_node({{true, true, true}}, {{x1, y1, phi1}});

        BeamElement element01(node0, node1, 0.0, L);
        element01.set_stiffness(EA, EI, 0.0);
        system.add_element(element01);

        check_stiffness_matrix(system);
    };

    check_at(0.0, 0.0, 0.0, 1.0, 0.0, 0.0);

    check_at( 0.0, -6.0,  1.0, -7.0, -2.0,  3.0);
    check_at( 4.0, -7.0, -5.0, -9.0,  6.0, -6.0);
    check_at( 9.0, -4.0,  0.0,  4.0,  2.0, -7.0);
    check_at( 2.0,  0.0,  5.0, 10.0, -1.0,  8.0);
    check_at(-6.0,  5.0,  4.0, -6.0, -6.0, -5.0);
    check_at(-4.0, -6.0, -4.0,  5.0,  0.0,  5.0);
}

