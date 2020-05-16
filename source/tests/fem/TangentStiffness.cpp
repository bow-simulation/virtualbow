#include "solver/fem/System.hpp"
#include "solver/fem/elements/BarElement.hpp"
#include "solver/fem/elements/BeamElement.hpp"
#include "solver/fem/elements/ContactElement.hpp"
#include "solver/model/BeamUtils.hpp"
#include <catch2/catch.hpp>
#include <iostream>

// Numeric tangent stiffness matrix using central differences
MatrixXd numeric_tangent_stiffness(System& system, double h = 1e-8)
{
    MatrixXd K = MatrixXd::Zero(system.dofs(), system.dofs());

    VectorXd q_fwd = VectorXd::Zero(system.dofs());
    VectorXd q_bwd = VectorXd::Zero(system.dofs());

    for(std::size_t i = 0; i < system.dofs(); ++i)
    {
        VectorXd u = system.get_u();
        double ui = u(i);

        u(i) = ui + h;
        system.set_u(u);
        q_fwd = system.get_q();

        u(i) = ui - h;
        system.set_u(u);
        q_bwd = system.get_q();

        u(i) = ui;
        system.set_u(u);
        K.col(i) = (q_fwd - q_bwd)/(2.0*h);
    }

    return K;
}

void check_stiffness_matrix(System& system)
{
    MatrixXd K_ana = system.get_K();
    MatrixXd K_num = numeric_tangent_stiffness(system);

    REQUIRE(K_ana.isApprox(K_num, 1e-5));                   // Compare to numeric derivatives
    REQUIRE(K_ana.isApprox(K_ana.transpose(), 1e-5));       // Check symmetry
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
        system.mut_elements().add(BarElement(system, node0, node1, L, EA, 0.0, 0.0));

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

        Matrix<6, 6> K = BeamUtils::stiffness_matrix(EA, EI, L, 0.0);
        BeamElement element01(system, node0, node1, K, Vector<6>::Zero());
        system.mut_elements().add(element01);

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

TEST_CASE("tangent-stiffness-contact-element")
{
    /*
    auto check_at = [](double x0, double y0, double phi0, double x1, double y1, double phi1, double x2, double y2)
    {
        double EA = 100.0;
        double EI = 10.0;

        System system;
        Node node0 = system.create_node({true, true,  true}, {x0, y0, phi0});
        Node node1 = system.create_node({true, true,  true}, {x1, y1, phi1});
        Node node2 = system.create_node({true, true, false}, {x2, y2,  0.0});

        ContactElement element(system, node0, node1, node2, 0.5, 0.25, {5000.0, 1e-4});
        system.mut_elements().add(element);

        check_stiffness_matrix(system);
    };

    check_at(0.0, 0.0, M_PI_2, 0.0, 1.0, M_PI_2, 1.25, 0.5);  // No contact
    check_at(0.0, 0.0, M_PI_2, 0.0, 1.0, M_PI_2, 0.25, 0.5);  // Contact
    */
}
