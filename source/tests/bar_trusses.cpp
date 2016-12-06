#include "../fem/System.hpp"
#include "../fem/elements/BarElement.hpp"

#include <catch.hpp>
#include <iostream>

TEST_CASE("small-deformation-bar-truss")
{
    double L = 0.5;
    double EA = 21000.0;    // Steel rod, 1cm x 1cm
    double F = 10.0;

    System system;
    Node node_01 = system.create_node({{false, false, false}}, {{  0.0, 0.0, 0.0}});
    Node node_02 = system.create_node({{ true,  true, false}}, {{    L, 0.0, 0.0}});
    Node node_03 = system.create_node({{ true,  true, false}}, {{2.0*L, 0.0, 0.0}});
    Node node_04 = system.create_node({{ true,  true, false}}, {{3.0*L, 0.0, 0.0}});
    Node node_05 = system.create_node({{ true, false, false}}, {{4.0*L, 0.0, 0.0}});
    Node node_06 = system.create_node({{ true,  true, false}}, {{  0.0,   L, 0.0}});
    Node node_07 = system.create_node({{ true,  true, false}}, {{    L,   L, 0.0}});
    Node node_08 = system.create_node({{ true,  true, false}}, {{2.0*L,   L, 0.0}});
    Node node_09 = system.create_node({{ true,  true, false}}, {{3.0*L,   L, 0.0}});
    Node node_10 = system.create_node({{ true,  true, false}}, {{4.0*L,   L, 0.0}});

    BarElement element_01_02(node_01, node_02, L, EA, 0.0, 0.0);
    BarElement element_02_03(node_02, node_03, L, EA, 0.0, 0.0);
    BarElement element_03_04(node_03, node_04, L, EA, 0.0, 0.0);
    BarElement element_04_05(node_04, node_05, L, EA, 0.0, 0.0);

    BarElement element_06_07(node_06, node_07, L, EA, 0.0, 0.0);
    BarElement element_07_08(node_07, node_08, L, EA, 0.0, 0.0);
    BarElement element_08_09(node_08, node_09, L, EA, 0.0, 0.0);
    BarElement element_09_10(node_09, node_10, L, EA, 0.0, 0.0);

    BarElement element_01_06(node_01, node_06, L, EA, 0.0, 0.0);
    BarElement element_02_07(node_02, node_07, L, EA, 0.0, 0.0);
    BarElement element_03_08(node_03, node_08, L, EA, 0.0, 0.0);
    BarElement element_04_09(node_04, node_09, L, EA, 0.0, 0.0);
    BarElement element_05_10(node_05, node_10, L, EA, 0.0, 0.0);

    BarElement element_01_07(node_01, node_07, std::sqrt(2)*L, EA, 0.0, 0.0);
    BarElement element_07_03(node_07, node_03, std::sqrt(2)*L, EA, 0.0, 0.0);
    BarElement element_03_09(node_03, node_09, std::sqrt(2)*L, EA, 0.0, 0.0);
    BarElement element_09_05(node_09, node_05, std::sqrt(2)*L, EA, 0.0, 0.0);

    system.elements().add(element_01_02);
    system.elements().add(element_02_03);
    system.elements().add(element_03_04);
    system.elements().add(element_04_05);
    system.elements().add(element_06_07);
    system.elements().add(element_07_08);
    system.elements().add(element_08_09);
    system.elements().add(element_09_10);
    system.elements().add(element_01_06);
    system.elements().add(element_02_07);
    system.elements().add(element_03_08);
    system.elements().add(element_04_09);
    system.elements().add(element_05_10);
    system.elements().add(element_01_07);
    system.elements().add(element_07_03);
    system.elements().add(element_03_09);
    system.elements().add(element_09_05);

    system.get_p(node_02.y) = -F;
    system.get_p(node_04.y) = -F;

    system.solve_statics_lc();

    double s_numeric = -system.get_u(node_03.y);
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

    System system;

    Node node01 = system.create_node({{false, false, false}}, {{  0.0, 0.0, 0.0}});
    Node node02 = system.create_node({{false, true,  false}}, {{    H,   H, 0.0}});
    Node node03 = system.create_node({{false, false, false}}, {{2.0*H, 0.0, 0.0}});

    BarElement element_01_02(node01, node02, M_SQRT2*H, EA, 0.0, 0.0);
    BarElement element_02_03(node02, node03, M_SQRT2*H, EA, 0.0, 0.0);
    system.elements().add(element_01_02);
    system.elements().add(element_02_03);

    // Solve for static equilibrium and get vertical deflection of node 02
    system.solve_statics_dc(node02.y, 0.6*H, 100, [&]()
    {
        double s = system.get_u(node02.y);
        double f_num = system.get_p(node02.y);

        double L0 = M_SQRT2*H;
        double L = std::hypot(H, s);
        double f_ref = 2.0*EA*s*(L - L0)/(L*L0);

        double error = std::abs(f_num - f_ref);     // Absolute error because f_ref = 0 initially

        REQUIRE(error < 1e-9);
        return true;
    });
}
