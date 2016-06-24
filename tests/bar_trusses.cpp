#include "../src/fem/System.hpp"
#include "../src/fem/elements/BarElement.hpp"

#include <catch.hpp>
#include <iostream>

TEST_CASE("Small deformation bar truss")
{
    double L = 0.5;
    double EA = 21000.0;    // Steel rod, 1cm x 1cm
    double F = 10.0;

    System system;
    Node node_01 = system.create_node({{  0.0, 0.0, 0.0}}, {{false, false, false}});
    Node node_02 = system.create_node({{    L, 0.0, 0.0}}, {{ true,  true, false}});
    Node node_03 = system.create_node({{2.0*L, 0.0, 0.0}}, {{ true,  true, false}});
    Node node_04 = system.create_node({{3.0*L, 0.0, 0.0}}, {{ true,  true, false}});
    Node node_05 = system.create_node({{4.0*L, 0.0, 0.0}}, {{ true, false, false}});
    Node node_06 = system.create_node({{  0.0,   L, 0.0}}, {{ true,  true, false}});
    Node node_07 = system.create_node({{    L,   L, 0.0}}, {{ true,  true, false}});
    Node node_08 = system.create_node({{2.0*L,   L, 0.0}}, {{ true,  true, false}});
    Node node_09 = system.create_node({{3.0*L,   L, 0.0}}, {{ true,  true, false}});
    Node node_10 = system.create_node({{4.0*L,   L, 0.0}}, {{ true,  true, false}});

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

    system.add_element(element_01_02);
    system.add_element(element_02_03);
    system.add_element(element_03_04);
    system.add_element(element_04_05);
    system.add_element(element_06_07);
    system.add_element(element_07_08);
    system.add_element(element_08_09);
    system.add_element(element_09_10);
    system.add_element(element_01_06);
    system.add_element(element_02_07);
    system.add_element(element_03_08);
    system.add_element(element_04_09);
    system.add_element(element_05_10);
    system.add_element(element_01_07);
    system.add_element(element_07_03);
    system.add_element(element_03_09);
    system.add_element(element_09_05);

    system.set_external_force(node_02.y, -F);
    system.set_external_force(node_04.y, -F);

    system.solve_statics_lc();

    double s_numeric = -system.get_u()(node_03.y);
    double s_analytic = (4.0+2.0*std::sqrt(2))*F*L/EA;

    REQUIRE(std::abs(s_numeric - s_analytic) < 1e-6);
}
