#include "solver/numerics/FindInterval.hpp"
#include <catch2/catch.hpp>
#include <algorithm>

TEST_CASE("find-interval")
{
    std::vector<double> x = {1.0, 2.0, 3.0, 4.0};

    REQUIRE(find_interval(x, 0.5) == 0);
    REQUIRE(find_interval(x, 1.5) == 0);
    REQUIRE(find_interval(x, 2.5) == 1);
    REQUIRE(find_interval(x, 3.5) == 2);
    REQUIRE(find_interval(x, 4.5) == 2);

    REQUIRE(find_interval(x, 2.5, -5) == 1);
    REQUIRE(find_interval(x, 2.5, 0) == 1);
    REQUIRE(find_interval(x, 2.5, 1) == 1);
    REQUIRE(find_interval(x, 2.5, 2) == 1);
    REQUIRE(find_interval(x, 2.5, 5) == 1);
}
