#include "ProfileCurve.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Fresnel.hpp"
#include "solver/numerics/FindInterval.hpp"
#include "solver/numerics/Sorting.hpp"

ProfileCurve2::ProfileCurve2(const ProfileInput& input2, double x0, double y0, double phi0)
{
    MatrixXd input{{0.0, 0.0}, {0.8, 0.0}};

    if(input.cols() != 2) {
        throw std::invalid_argument("Input must have two columns");
    }

    std::vector<double> s;
    std::vector<double> k;

    // Extract s and k from input, ignore rows that contain NaN
    for(int i = 0; i < input.rows(); ++i)
    {
        double si = input(i, 0);
        double ki = input(i, 1);
        if(!std::isnan(si) && !std::isnan(ki))
        {
            s.push_back(si);
            k.push_back(ki);
        }
    }

    if(s.size() < 2) {
        throw std::invalid_argument("Input must have at least two valid rows");
    }

    // Sort inputs
    sort_by_argument(s, k);
    this->s = s;

    CurvePoint p0 = {.k = 0.0, .phi = phi0, .x = x0, .y = y0};
    for(size_t i = 0; i < s.size() - 1; ++i)
    {
        segment_fn spiral = euler_spiral(s[i], s[i+1], k[i], k[i+1], p0.x, p0.y, p0.phi);
        p0 = spiral(s[i+1]);
        f.push_back(spiral);
    }
}

double ProfileCurve2::s_min() const
{
    return s.front();
}

double ProfileCurve2::s_max() const
{
    return s.back();
}

// Extrapolates on out of bounds access
CurvePoint ProfileCurve2::operator()(double arg) const
{
    index = find_interval(s, arg, index);
    return f[index](arg);
}

ProfileCurve2::segment_fn ProfileCurve2::euler_spiral(double s0, double s1, double k0, double k1, double x0, double y0, double phi0)
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
        return [=](double s) -> CurvePoint {
            double Ss, Cs, Sc, Cc;
            std::tie(Ss, Cs) = fresnel((b + 2.0*a*s)/sqrt(2.0*M_PI*a));
            std::tie(Sc, Cc) = fresnel((b + 2.0*a*s0)/sqrt(2.0*M_PI*a));
            return {
                .k = 2.0*a*s + b,
                .phi = a*s*s + b*s + c,
                .x = x0 + sqrt(M_PI_2/a)*(cos(b*b/(4.0*a) - c)*(Cs - Cc) + sin(b*b/(4.0*a) - c)*(Ss - Sc)),
                .y = y0 + sqrt(M_PI_2/a)*(sin(b*b/(4.0*a) - c)*(Cc - Cs) + cos(b*b/(4.0*a) - c)*(Ss - Sc))
            };
        };
    }
    else if(a < 0.0)
    {
        // Euler spiral with decreasing curvature
        // Solution from above, but with k0 = -k0, k1 = -k1, y = -y
        return [=](double s) -> CurvePoint {
            double Ss, Cs, Sc, Cc;
            std::tie(Ss, Cs) = fresnel((b + 2.0*a*s)/sqrt(-2.0*M_PI*a));
            std::tie(Sc, Cc) = fresnel((b + 2.0*a*s0)/sqrt(-2.0*M_PI*a));
            return {
                .k = 2.0*a*s + b,
                .phi = a*s*s + b*s + c,
                .x = x0 - sqrt(-M_PI_2/a)*(cos(c - b*b/(4.0*a))*(Cs - Cc) + sin(c - b*b/(4.0*a))*(Ss - Sc)),
                .y = y0 + sqrt(-M_PI_2/a)*(sin(c - b*b/(4.0*a))*(Cc - Cs) + cos(c - b*b/(4.0*a))*(Ss - Sc)),
            };
        };
    }
    else if(b != 0.0)
    {
        // Circle (constant curvature)
        // x(s) = x0 + integrate cos(b*t + c) dt from s0 to s
        // y(s) = y0 + integrate sin(b*t + c) dt from s0 to s
        return [=](double s) -> CurvePoint {
            return {
                .k = b,
                .phi = b*s + c,
                .x = x0 + 1/b*(sin(b*s + c) - sin(b*s0 + c)),
                .y = y0 + 1/b*(cos(b*s0 + c) - cos(b*s + c))
            };
        };
    }
    else
    {
        // Line (zero curvature)
        // x(s) = x0 + integrate cos(c) dt from s0 to s
        // y(s) = y0 + integrate sin(c) dt from s0 to s
        return [=](double s) -> CurvePoint {
            return {
                .k = 0.0,
                .phi = c,
                .x = x0 + cos(c)*(s - s0),
                .y = y0 + sin(c)*(s - s0)
            };
        };
    }
}
