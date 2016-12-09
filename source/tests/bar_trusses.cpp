#include "../fem/System2.hpp"
#include "../fem/Solver.hpp"
#include "../fem/elements/BarElement2.hpp"

#include <catch.hpp>
#include <iostream>

TEST_CASE("small-deformation-bar-truss")
{
    double L = 0.5;
    double EA = 21000.0;    // Steel rod, 1cm x 1cm
    double F = 10.0;

    System2 system;
    Node2 node_01 = system.create_node({DofType::Fixed,  DofType::Fixed,  DofType::Fixed}, {  0.0, 0.0, 0.0});
    Node2 node_02 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {    L, 0.0, 0.0});
    Node2 node_03 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {2.0*L, 0.0, 0.0});
    Node2 node_04 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {3.0*L, 0.0, 0.0});
    Node2 node_05 = system.create_node({DofType::Active, DofType::Fixed,  DofType::Fixed}, {4.0*L, 0.0, 0.0});
    Node2 node_06 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {  0.0,   L, 0.0});
    Node2 node_07 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {    L,   L, 0.0});
    Node2 node_08 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {2.0*L,   L, 0.0});
    Node2 node_09 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {3.0*L,   L, 0.0});
    Node2 node_10 = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {4.0*L,   L, 0.0});

    system.add_element(BarElement2(node_01, node_02, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_02, node_03, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_03, node_04, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_04, node_05, L, EA, 0.0, 0.0));

    system.add_element(BarElement2(node_06, node_07, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_07, node_08, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_08, node_09, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_09, node_10, L, EA, 0.0, 0.0));

    system.add_element(BarElement2(node_01, node_06, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_02, node_07, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_03, node_08, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_04, node_09, L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_05, node_10, L, EA, 0.0, 0.0));

    system.add_element(BarElement2(node_01, node_07, std::sqrt(2)*L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_07, node_03, std::sqrt(2)*L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_03, node_09, std::sqrt(2)*L, EA, 0.0, 0.0));
    system.add_element(BarElement2(node_09, node_05, std::sqrt(2)*L, EA, 0.0, 0.0));

    node_02[1].p() = -F;
    node_04[1].p() = -F;

    StaticSolverLC solver(system);
    solver.find_equilibrium();

    double s_numeric = -node_03[1].u();
    double s_analytic = (4.0+2.0*std::sqrt(2))*F*L/EA;

    REQUIRE(std::abs(s_numeric - s_analytic) < 1e-6);
}

// Todo: Why does the displacement control not allow passing the point 0.5*H?
// Read section on displacement control in 'Nonlinear Finite Element Analysis of Solids and Structures (René De Borst,Mike A. Crisfield,Joris J. C.)
TEST_CASE("large-deformation-bar-truss")
{
    double H = 1.0;
    double EA = 10000.0;
    double F = 1500.0;

    System2 system;

    Node2 node01 = system.create_node({DofType::Fixed, DofType::Fixed,  DofType::Fixed}, {  0.0, 0.0, 0.0});
    Node2 node02 = system.create_node({DofType::Fixed, DofType::Active, DofType::Fixed}, {    H,   H, 0.0});
    Node2 node03 = system.create_node({DofType::Fixed, DofType::Fixed,  DofType::Fixed}, {2.0*H, 0.0, 0.0});

    system.add_element(BarElement2(node01, node02, M_SQRT2*H, EA, 0.0, 0.0));
    system.add_element(BarElement2(node02, node03, M_SQRT2*H, EA, 0.0, 0.0));

    StaticSolverDC solver(system, node02[1], 0.6*H, 100);

    while(solver.step())
    {
        double s = node02[1].u();
        double f_num = node02[1].p();

        double L0 = M_SQRT2*H;
        double L = std::hypot(H, s);
        double f_ref = 2.0*EA*s*(L - L0)/(L*L0);

        double error = std::abs(f_num - f_ref);     // Absolute error because f_ref = 0 initially

        REQUIRE(error < 1e-9);
    }
}
