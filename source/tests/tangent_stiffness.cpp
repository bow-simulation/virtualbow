#include "../fem/System.hpp"
#include "../fem/elements/BarElement2.hpp"
#include "../fem/elements/BeamElement2.hpp"

#include <catch.hpp>
#include <iostream>

// Numeric tangent stiffness matrix using central differences
Eigen::MatrixXd numeric_tangent_stiffness(System& system, double h = 1e-8)
{
    Eigen::MatrixXd K = Eigen::MatrixXd::Zero(system.dofs(), system.dofs());

    Eigen::VectorXd q_fwd = Eigen::VectorXd::Zero(system.dofs());
    Eigen::VectorXd q_bwd = Eigen::VectorXd::Zero(system.dofs());

    for(std::size_t i = 0; i < system.dofs(); ++i)
    {
        double ui = system.u()(i);

        system.u_mut()(i) = ui + h;
        q_fwd = system.q();

        system.u_mut()(i) = ui - h;
        q_bwd = system.q();

        system.u_mut()(i) = ui;
        K.col(i) = (q_fwd - q_bwd)/(2.0*h);
    }

    return K;
}

void check_stiffness_matrix(System& system)
{
    Eigen::MatrixXd K_ana = system.K();
    Eigen::MatrixXd K_num = numeric_tangent_stiffness(system);

    REQUIRE(K_ana.isApprox(K_num, 1e-5));                   // Equal to numeric derivatives
    REQUIRE(K_ana.isApprox(K_num.transpose(), 1e-5));       // Symmetric
}

TEST_CASE("tangent-stiffness-bar-element")
{
    auto check_at = [](double dx0, double dy0, double dx1, double dy1)
    {
        double L = 1.0;
        double EA = 100.0;

        System system;
        Node node0 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {    dx0, dy0, 0.0});
        Node node1 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {L + dx1, dy1, 0.0});
        system.add_element(BarElement2(node0, node1, L, EA, 0.0, 0.0));

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

TEST_CASE("tangent-stiffness-beam-element")
{
    auto check_at = [](double x0, double y0, double phi0, double x1, double y1, double phi1)
    {
        double L = 1.0;
        double EA = 100.0;
        double EI = 10.0;

        System system;
        Node node0 = system.create_node({DofType::Active, DofType::Active, DofType::Active}, {x0, y0, phi0});
        Node node1 = system.create_node({DofType::Active, DofType::Active, DofType::Active}, {x1, y1, phi1});

        BeamElement2 element01(node0, node1, 0.0, L);
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

