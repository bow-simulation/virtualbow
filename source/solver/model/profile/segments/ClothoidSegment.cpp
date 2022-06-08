#include "ClothoidSegment.hpp"
#include "solver/numerics/Fresnel.hpp"
#include <cmath>

double radius_to_curvature(double r) {
    if(r != 0.0) {
        return 1.0/r;
    }
    else {
        return 0.0;
    }
}

ClothoidSegment::ClothoidSegment(const Point& start, const SpiralInput& input)
    : ClothoidSegment(start, input.at(SpiralConstraint::LENGTH),
                      radius_to_curvature(input.at(SpiralConstraint::R_START)),
                      radius_to_curvature(input.at(SpiralConstraint::R_END)))
{

}

ClothoidSegment::ClothoidSegment(const Point& start, const ArcInput& input)
    : ClothoidSegment(start, input.at(ArcConstraint::LENGTH),
                      radius_to_curvature(input.at(ArcConstraint::R_START)),
                      radius_to_curvature(input.at(ArcConstraint::R_START)))
{

}

ClothoidSegment::ClothoidSegment(const Point& start, const LineInput& input)
    : ClothoidSegment(start, input.at(LineConstraint::LENGTH), 0.0, 0.0) {

}

ClothoidSegment::ClothoidSegment(const Point& start, double l, double k0, double k1)
    : x0(start.position[0]),
      y0(start.position[1]),
      s0(start.s),
      s1(start.s + l),
      a(0.5*(k1 - k0)/(s1 - s0)),
      b((s1*k0 - s0*k1)/(s1 - s0)),
      c(start.angle - b*s0 - a*s0*s0)
{

}

double ClothoidSegment::s_start() const {
    return s0;
}

double ClothoidSegment::s_end() const {
    return s1;
}

double ClothoidSegment::curvature(double s) const {
    return 2.0*a*s + b;
}

double ClothoidSegment::angle(double s) const {
    return (a*s + b)*s + c;
}

// Curvature varies linearly between s0 and s1:
// k(s) = (k1 - k0)/(s1 - s0)*s + (s1*k0 - s0*k1)/(s1 - s0)
//
// Therefore, the tangent angle is (https://en.wikipedia.org/wiki/Tangential_angle)
// phi(s) = phi0 + integrate k(t) dt from s0 to s
//        = phi0 + (s1*k0 - s0*k1)/(s1 - s0)*(s - s0) + 1/2*(k1 - k0)/(s1 - s0)*(s^2 - s0^2)
//        = a*s^2 + b*s + c
//
// Quadratic coefficients of the tangent angle:
//
// a = 0.5*(k1 - k0)/(s1 - s0);
// b = (s1*k0 - s0*k1)/(s1 - s0);
// c = phi0 - b*s0 - a*s0*s0;

Vector<2> ClothoidSegment::position(double s) const {
    // The curve's coordinates are (https://en.wikipedia.org/wiki/Tangential_angle)
    // x(s) = x0 + integrate cos(phi(t)) dt from s0 to s
    // y(s) = y0 + integrate sin(phi(t)) dt from s0 to s
    // The solution to this depends on the values of the coefficients
    if(a > 0.0) {
        // Euler spiral with increasing curvature
        // x(s) = x0 + integrate cos(a*t^2 + b*t + c) dt from s0 to s
        // y(s) = y0 + integrate sin(a*t^2 + b*t + c) dt from s0 to s
        double Ss, Cs, Sc, Cc;
        std::tie(Ss, Cs) = fresnel((b + 2.0*a*s)/sqrt(2.0*M_PI*a));
        std::tie(Sc, Cc) = fresnel((b + 2.0*a*s0)/sqrt(2.0*M_PI*a));
        return {
            x0 + sqrt(M_PI_2/a)*(cos(b*b/(4.0*a) - c)*(Cs - Cc) + sin(b*b/(4.0*a) - c)*(Ss - Sc)),
                    y0 + sqrt(M_PI_2/a)*(sin(b*b/(4.0*a) - c)*(Cc - Cs) + cos(b*b/(4.0*a) - c)*(Ss - Sc))
        };
    }
    else if(a < 0.0) {
        // Euler spiral with decreasing curvature
        // Solution from above, but with k0 = -k0, k1 = -k1, y = -y
        double Ss, Cs, Sc, Cc;
        std::tie(Ss, Cs) = fresnel((b + 2.0*a*s)/sqrt(-2.0*M_PI*a));
        std::tie(Sc, Cc) = fresnel((b + 2.0*a*s0)/sqrt(-2.0*M_PI*a));
        return {
            x0 - sqrt(-M_PI_2/a)*(cos(c - b*b/(4.0*a))*(Cs - Cc) + sin(c - b*b/(4.0*a))*(Ss - Sc)),
                    y0 + sqrt(-M_PI_2/a)*(sin(c - b*b/(4.0*a))*(Cc - Cs) + cos(c - b*b/(4.0*a))*(Ss - Sc))
        };
    }
    else if(b != 0.0) {
        // Circle (constant curvature)
        // x(s) = x0 + integrate cos(b*t + c) dt from s0 to s
        // y(s) = y0 + integrate sin(b*t + c) dt from s0 to s
        return {
            x0 + 1/b*(sin(b*s + c) - sin(b*s0 + c)),
                    y0 + 1/b*(cos(b*s0 + c) - cos(b*s + c))
        };
    }
    else {
        // Line (zero curvature)
        // x(s) = x0 + integrate cos(c) dt from s0 to s
        // y(s) = y0 + integrate sin(c) dt from s0 to s
        return {
            x0 + cos(c)*(s - s0),
                    y0 + sin(c)*(s - s0)
        };
    }
}
