#include "solver/fem/System.hpp"
#include "solver/fem/StaticSolver.hpp"
#include "solver/fem/elements/BarElement.hpp"
#include "solver/numerics/Linspace.hpp"
#include <catch2/catch.hpp>
#include <iostream>

/*
TEST_CASE("small-deformation-bar-truss")
{
    double L = 0.5;
    double EA = 21000.0;    // Steel rod, 1cm x 1cm
    double F = 10.0;

    System system;
    Node node_01 = system.create_node({false, false, false}, {  0.0, 0.0, 0.0});
    Node node_02 = system.create_node({ true,  true, false}, {    L, 0.0, 0.0});
    Node node_03 = system.create_node({ true,  true, false}, {2.0*L, 0.0, 0.0});
    Node node_04 = system.create_node({ true,  true, false}, {3.0*L, 0.0, 0.0});
    Node node_05 = system.create_node({ true, false, false}, {4.0*L, 0.0, 0.0});
    Node node_06 = system.create_node({ true,  true, false}, {  0.0,   L, 0.0});
    Node node_07 = system.create_node({ true,  true, false}, {    L,   L, 0.0});
    Node node_08 = system.create_node({ true,  true, false}, {2.0*L,   L, 0.0});
    Node node_09 = system.create_node({ true,  true, false}, {3.0*L,   L, 0.0});
    Node node_10 = system.create_node({ true,  true, false}, {4.0*L,   L, 0.0});

    system.mut_elements().add(BarElement(system, node_01, node_02, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_02, node_03, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_03, node_04, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_04, node_05, L, EA, 0.0, 0.0));

    system.mut_elements().add(BarElement(system, node_06, node_07, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_07, node_08, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_08, node_09, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_09, node_10, L, EA, 0.0, 0.0));

    system.mut_elements().add(BarElement(system, node_01, node_06, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_02, node_07, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_03, node_08, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_04, node_09, L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_05, node_10, L, EA, 0.0, 0.0));

    system.mut_elements().add(BarElement(system, node_01, node_07, std::sqrt(2)*L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_07, node_03, std::sqrt(2)*L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_03, node_09, std::sqrt(2)*L, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node_09, node_05, std::sqrt(2)*L, EA, 0.0, 0.0));

    system.set_p(node_02.y, -F);
    system.set_p(node_04.y, -F);

    StaticSolverLC solver(system);
    solver.solve();

    double s_num = -system.get_u(node_03.y);
    double s_ref = (4.0 + 2.0*std::sqrt(2))*F*L/EA;

    REQUIRE(std::abs(s_num - s_ref) < 1e-6);
}
*/

// Todo: Why does the displacement control not allow passing the point 0.5*H?
// Read section on displacement control in 'Nonlinear Finite Element Analysis of Solids and Structures (René De Borst,Mike A. Crisfield,Joris J. C.)
TEST_CASE("large-deformation-bar-truss")
{
    double H = 1.0;
    double EA = 10000.0;

    System system;

    Node node01 = system.create_node({DofType::Fixed, DofType::Fixed , DofType::Fixed}, {  0.0, 0.0, 0.0});
    Node node02 = system.create_node({DofType::Fixed, DofType::Active, DofType::Fixed}, {    H,   H, 0.0});
    Node node03 = system.create_node({DofType::Fixed, DofType::Fixed , DofType::Fixed}, {2.0*H, 0.0, 0.0});

    system.mut_elements().add(BarElement(system, node01, node02, M_SQRT2*H, EA, 0.0, 0.0));
    system.mut_elements().add(BarElement(system, node02, node03, M_SQRT2*H, EA, 0.0, 0.0));

    StaticSolver solver(system, node02.y, StaticSolver::Settings());
    solver.solve_equilibrium_path(-H, 10, [&](){
        double s = system.get_u(node02.y);
        double F_num = system.get_p(node02.y);

        // Analytical solution
        double L0 = M_SQRT2*H;
        double L = hypot(H, s);
        double F_ref = 2.0*EA*s*(L - L0)/(L*L0);

        std::cout << "s = " << s << "\n";

        // Error
        REQUIRE(std::abs(F_num - F_ref) < 1e-9);

        return true;
    });
}
