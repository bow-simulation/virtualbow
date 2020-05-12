#include "numerics/Integration.hpp"
#include <catch.hpp>

#include <iostream>

TEST_CASE("adaptive-simpson-method")
{
    auto f = [](double x) {
        return (Vector<3>() << x*x, sin(2*M_PI*x), 1.0/(x + 1.0)).finished();
    };

    Vector<3> I_num = AdaptiveSimpson::integrate<Vector<3>>(f, 0.0, 1.0, 1e-9);
    Vector<3> I_ref = (Vector<3>() << 1.0/3.0, 0.0, log(2.0)).finished();

    REQUIRE(I_num.isApprox(I_ref, 1e-15));
}
