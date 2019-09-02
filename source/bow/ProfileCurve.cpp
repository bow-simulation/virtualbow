#include "ProfileCurve.hpp"
#include "numerics/Eigen.hpp"
#include "numerics/Fresnel.hpp"
#include "numerics/FindInterval.hpp"
#include "numerics/Sorting.hpp"

ProfileCurve::ProfileCurve(std::vector<double> s, std::vector<double> k, double x0, double y0, double phi0)
{
    if(s.size() < 2)
        throw std::invalid_argument("At least two arc lengths are needed");
    if(s.size() != k.size())
        throw std::invalid_argument("Argument length mismatch");

    // Sort inputs
    sort_by_argument(s, k);
    this->s = s;

    Vector<3> r0 = {x0, y0, phi0};
    for(size_t i = 0; i < s.size() - 1; ++i)
    {
        segment_fn spiral = euler_spiral(s[i], s[i+1], k[i], k[i+1], r0[0], r0[1], r0[2]);
        r0 = spiral(s[i+1]);
        f.push_back(spiral);
    }
}

double ProfileCurve::s_min() const
{
    return s.front();
}

double ProfileCurve::s_max() const
{
    return s.back();
}

Vector<3> ProfileCurve::operator()(double arg) const
{
    assert(arg >= s_min());
    assert(arg <= s_max());

    index = find_interval(s, arg, index);
    return f[index](arg);
}

ProfileCurve::segment_fn ProfileCurve::euler_spiral(double s0, double s1, double k0, double k1, double x0, double y0, double phi0)
{
    // Curvature varies linearly between s0 and s1:
    // k(s) = (k1 - k0)/(s1 - s0)*s + (s1*k0 - s0*k1)/(s1 - s0)
    //
    // Therefore, the tangent angle is (https://en.wikipedia.org/wiki/Tangential_angle)
    // phi(s) = phi0 + integrate k(t) dt from s0 to s
    //        = phi0 + (s1*k0 - s0*k1)/(s1 - s0)*(s - s0) + 1/2*(k1 - k0)/(s1 - s0)*(s^2 - s0^2)
    //        = a*s^2 + b*s + c

    // Quadratic coefficients of the tangent angle:
    double a = 0.5*(k1 - k0)/(s1 - s0);
    double b = (s1*k0 - s0*k1)/(s1 - s0);
    double c = phi0 - b*s0 - a*s0*s0;

    // The curve's coordinates are (https://en.wikipedia.org/wiki/Tangential_angle)
    // x(s) = x0 + integrate cos(phi(t)) dt from s0 to s
    // y(s) = y0 + integrate sin(phi(t)) dt from s0 to s
    // The solution to this depends on the values of the coefficients
    if(a > 0.0)
    {
        // Euler spiral with increasing curvature
        // x(s) = x0 + integrate cos(a*t^2 + b*t + c) dt from s0 to s
        // y(s) = y0 + integrate sin(a*t^2 + b*t + c) dt from s0 to s
        return [=](double s) -> Vector<3> {
            double Ss, Cs, Sc, Cc;
            std::tie(Ss, Cs) = fresnel((b + 2.0*a*s)/sqrt(2.0*M_PI*a));
            std::tie(Sc, Cc) = fresnel((b + 2.0*a*s0)/sqrt(2.0*M_PI*a));
            return {
                x0 + sqrt(M_PI_2/a)*(cos(b*b/(4.0*a) - c)*(Cs - Cc) + sin(b*b/(4.0*a) - c)*(Ss - Sc)),
                y0 + sqrt(M_PI_2/a)*(sin(b*b/(4.0*a) - c)*(Cc - Cs) + cos(b*b/(4.0*a) - c)*(Ss - Sc)),
                a*s*s + b*s + c
            };
        };
    }
    else if(a < 0.0)
    {
        // Euler spiral with decreasing curvature
        // Solution from above, but with k0 = -k0, k1 = -k1, y = -y
        return [=](double s) -> Vector<3> {
            double Ss, Cs, Sc, Cc;
            std::tie(Ss, Cs) = fresnel((b + 2.0*a*s)/sqrt(-2.0*M_PI*a));
            std::tie(Sc, Cc) = fresnel((b + 2.0*a*s0)/sqrt(-2.0*M_PI*a));
            return {
                x0 - sqrt(-M_PI_2/a)*(cos(c - b*b/(4.0*a))*(Cs - Cc) + sin(c - b*b/(4.0*a))*(Ss - Sc)),
                y0 + sqrt(-M_PI_2/a)*(sin(c - b*b/(4.0*a))*(Cc - Cs) + cos(c - b*b/(4.0*a))*(Ss - Sc)),
                a*s*s + b*s + c
            };
        };
    }
    else if(b != 0.0)
    {
        // Circle (constant curvature)
        // x(s) = x0 + integrate cos(b*t + c) dt from s0 to s
        // y(s) = y0 + integrate sin(b*t + c) dt from s0 to s
        return [=](double s) -> Vector<3> {
            return {
                x0 + 1/b*(sin(b*s + c) - sin(b*s0 + c)),
                y0 + 1/b*(cos(b*s0 + c) - cos(b*s + c)),
                b*s + c
            };
        };
    }
    else
    {
        // Line (zero curvature)
        // x(s) = x0 + integrate cos(c) dt from s0 to s
        // y(s) = y0 + integrate sin(c) dt from s0 to s
        return [=](double s) -> Vector<3> {
            return {
                x0 + cos(c)*(s - s0),
                y0 + sin(c)*(s - s0),
                c
            };
        };
    }
}
