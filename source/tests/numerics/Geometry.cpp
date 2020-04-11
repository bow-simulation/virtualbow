#include "solver/numerics/Geometry.hpp"

#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("is-right-handed")
{
    REQUIRE(get_orientation({0, 0}, {1, 0}, {0, 1}) == Orientation::RightHanded);
    REQUIRE(get_orientation({0, 0}, {0, 1}, {1, 0}) == Orientation::LeftHanded);
    REQUIRE(get_orientation({0, 0}, {1, 1}, {2, 2}) == Orientation::Collinear);
}

TEST_CASE("constant-orientation-subset")
{
    std::vector<Vector<2>> input, output;

    input  = {{0, 0}, {1, 0}};
    output = {{0, 0}, {1, 0}};
    REQUIRE(constant_orientation_subset<Orientation::RightHanded>(input) == output);

    input  = {{0, 0}, {1, 0}, {2, 1}};
    output = {{0, 0}, {1, 0}, {2, 1}};
    REQUIRE(constant_orientation_subset<Orientation::RightHanded>(input) == output);

    input  = {{0, 0}, {1, 0}, {2, -1}};
    output = {{0, 0}, {2, -1}};
    REQUIRE(constant_orientation_subset<Orientation::RightHanded>(input) == output);

    input  = {{0, 0}, {1, 0}, {2, 1}, {3, 3}};
    output = {{0, 0}, {1, 0}, {2, 1}, {3, 3}};
    REQUIRE(constant_orientation_subset<Orientation::RightHanded>(input) == output);

    input  = {{0, 0}, {1, 0}, {2, 1}, {3, 3}, {4, 3}};
    output = {{0, 0}, {1, 0}, {4, 3}};
    REQUIRE(constant_orientation_subset<Orientation::RightHanded>(input) == output);
}
