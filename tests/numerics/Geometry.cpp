#include "numerics/Geometry.hpp"
#include <catch.hpp>

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

TEST_CASE("points-with-distance-to-line")
{
    // No solution
    REQUIRE(points_by_distance_to_line({0.0, 0.0}, {1.0, 0.0}, {0.5, 0.5}, 0.4).hasNaN());

    // One solution
    REQUIRE(points_by_distance_to_line({0.0, 0.0}, {1.0, 0.0}, {0.5, 0.5}, 0.5).isApprox(Vector<2>{0.5, 0.5}));

    // Two solutions
    REQUIRE(points_by_distance_to_line({0.0, 0.0}, {1.0, 0.0}, {0.5, 0.5}, 0.5*sqrt(2)).isApprox(Vector<2>{1.0, 0.0}));

}

TEST_CASE("point_by_min_distance_to_line")
{
    // C off line, point in between
    REQUIRE(point_by_min_distance_to_line({0.0, 0.0}, {1.0, 1.0}, {1.0, 0.0}) == 0.5);

    // C off line, left of A
    REQUIRE(point_by_min_distance_to_line({0.0, 0.0}, {1.0, 1.0}, {-1.0, 0.0}) == -0.5);

    // C off line, right of B
    REQUIRE(point_by_min_distance_to_line({0.0, 0.0}, {1.0, 1.0}, {2.0, 1.0}) == 1.5);

    // C on line, point in between
    REQUIRE(point_by_min_distance_to_line({0.0, 0.0}, {1.0, 1.0}, {0.5, 0.5}) == 0.5);

    // C on line, point A
    REQUIRE(point_by_min_distance_to_line({1.0, 2.0}, {3.0, 4.0}, {1.0, 2.0}) == 0.0);

    // C on line, point B
    REQUIRE(point_by_min_distance_to_line({1.0, 2.0}, {3.0, 4.0}, {3.0, 4.0}) == 1.0);
}
