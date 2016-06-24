#include "../src/fem/System.hpp"
#include "../src/fem/elements/BarElement.hpp"

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
        Node node0 = system.create_node({{    dx0, dy0, 0.0}}, {{Dof::Type::Active, Dof::Type::Active, Dof::Type::Fixed}});
        Node node1 = system.create_node({{L + dx1, dy1, 0.0}}, {{Dof::Type::Active, Dof::Type::Active, Dof::Type::Fixed}});

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

