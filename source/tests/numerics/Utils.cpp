#include "numerics/Utils.hpp"
#include <catch.hpp>

TEST_CASE("sgn")
{
    REQUIRE(sgn( 1.0) == 1);
    REQUIRE(sgn(-1.0) == -1);
    REQUIRE(sgn( 0.0) == 0);
}

TEST_CASE("clamp")
{
    REQUIRE(clamp(1.0, 0.5, 1.5) == 1.0);
    REQUIRE(clamp(0.4, 0.5, 1.5) == 0.5);
    REQUIRE(clamp(1.6, 0.5, 1.5) == 1.5);

    REQUIRE(clamp(-1.0, -0.5, 0.5) == -0.5);
    REQUIRE(clamp( 0.0, -0.5, 0.5) ==  0.0);
    REQUIRE(clamp( 1.0, -0.5, 0.5) ==  0.5);

    REQUIRE(clamp(-1.0, -0.5, -1.5) == -1.0);
    REQUIRE(clamp(-0.4, -0.5, -1.5) == -0.5);
    REQUIRE(clamp(-1.6, -0.5, -1.5) == -1.5);
}
