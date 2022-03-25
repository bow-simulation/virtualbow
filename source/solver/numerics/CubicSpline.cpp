#include "CubicSpline.hpp"
#include "solver/numerics/FindInterval.hpp"
#include "solver/numerics/Sorting.hpp"
#include "solver/numerics/Utils.hpp"
#include "solver/numerics/TDMatrix.hpp"
#include <stdexcept>
#include <cmath>

CubicSpline::CubicSpline(const std::vector<double>& args, const std::vector<double>& vals, bool monotonic,
                         BoundaryType bd_type_left, double bd_value_left,
                         BoundaryType bd_type_right, double bd_value_right)
    : x(args), y(vals), m(args.size())
{
    if(x.size() != y.size()) {
        throw std::invalid_argument("x and y must have the same size");
    }

    size_t n = x.size();
    if(n < 2) {
        throw std::invalid_argument("Input must contain at least two points");
    }

    // Sort inputs, check for duplicates
    sort_by_argument(x, y);
    for(size_t i = 0; i < n-1; ++i) {
        if(!(x[i] < x[i+1])) {
            throw std::invalid_argument("Argument values must be distinct");
        }
    }

    // Determine slopes for C2 spline
    // m is the right hand side and is transformed to the resulting slopes by solving the system in-place
    TDMatrix<double> A(n);

    // Contuity conditions
    for(size_t i  = 1; i < n-1; ++i) {
        A.set(i, i-1, 1.0*(x[i] - x[i-1]));
        A.set(i, i  , 2.0*((x[i+1] - x[i]) + (x[i] - x[i-1])));
        A.set(i, i+1, 1.0*(x[i+1] - x[i]));
        m[i] = 3.0*((y[i+1] - y[i])/(x[i+1] - x[i])*(x[i] - x[i-1]) + (y[i] - y[i-1])/(x[i] - x[i-1])*(x[i+1] - x[i]));
    }

    // Boundary condition left
    switch(bd_type_left) {
        case BoundaryType::SECOND_DERIVATIVE:
            A.set(0, 0, 4.0);
            A.set(0, 1, 2.0);
            m[0] = 6.0*(y[1] - y[0])/(x[1] - x[0]) - (x[1] - x[0])*bd_value_left;
            break;

        case BoundaryType::FIRST_DERIVATIVE:
            A.set(0, 0, 1.0);
            m[0] = bd_value_left;
            break;
    }

    // Boundary condition right
    switch(bd_type_right) {
        case BoundaryType::SECOND_DERIVATIVE:
            A.set(n-1, n-2, 2.0);
            A.set(n-1, n-1, 4.0);
            m[n-1] = 6.0*(y[n-1] - y[n-2])/(x[n-1] - x[n-2]) + (x[n-1] - x[n-2])*bd_value_right;
            break;

        case BoundaryType::FIRST_DERIVATIVE:
            A.set(n-1, n-1, 1.0);
            m[n-1] = bd_value_right;
            break;
    }

    A.solve(m);  // Solve in-place

    // Optionally preserve monotonicity
    if(monotonic) {
        for(size_t i  = 0; i < n-1; ++i) {
            // Ensure that sgn(m_{i}) = sgn(m_{i+1}) = sgn(delta_{i})
            double delta = (y[i+1] - y[i])/(x[i+1] - x[i]);
            if(sgn(m[i]) != sgn(delta)) {
                m[i] = 0.0;
            }
            if(sgn(m[i+1]) != sgn(delta)) {
                m[i+1] = 0.0;
            }

            // Scale slopes to prevent overshoot
            double alpha = m[i]/delta;
            double beta = m[i+1]/delta;

            if(alpha*alpha + beta*beta > 9.0) {
                double tau = 3.0/std::hypot(alpha, beta);
                m[i] = tau*alpha*delta;
                m[i+1] = tau*beta*delta;
            }
        }
    }

}

std::vector<double> extract_x_values(const std::vector<Vector<2>>& input) {
    std::vector<double> values(input.size());
    for(size_t i = 0; i < input.size(); ++i) {
        values[i] = input[i][0];
    }
    return values;
}

std::vector<double> extract_y_values(const std::vector<Vector<2>>& input) {
    std::vector<double> values(input.size());
    for(size_t i = 0; i < input.size(); ++i) {
        values[i] = input[i][1];
    }
    return values;
}

CubicSpline::CubicSpline(const std::vector<Vector<2>>& input, bool monotonic,
                         BoundaryType bd_type_left, double bd_value_left,
                         BoundaryType bd_type_right, double bd_value_right)
    : CubicSpline(extract_x_values(input), extract_y_values(input), monotonic,
                  bd_type_left, bd_value_left,
                  bd_type_right, bd_value_right)
{

}

// Extrapolates on out of bounds access
double CubicSpline::operator()(double arg) const {
    // TODO: Horner evaluation
    auto h_00 = [](double t) { return 2*t*t*t - 3*t*t + 1; };
    auto h_10 = [](double t) { return t*t*t - 2*t*t + t; };
    auto h_01 = [](double t) { return -2*t*t*t + 3*t*t; };
    auto h_11 = [](double t) { return t*t*t - t*t; };

    index = find_interval(x, arg, index);
    double dx = x[index + 1] - x[index];
    double t = (arg - x[index])/dx;

    return h_00(t)*y[index] + h_10(t)*dx*m[index] + h_01(t)*y[index + 1] + h_11(t)*dx*m[index + 1];
}

// Returns default value on out of bounds access
double CubicSpline::operator()(double arg, double default_value) const {
    if(arg >= x.front() && arg <= x.back()) {
        return operator()(arg);
    } else {
        return default_value;
    }
}

double CubicSpline::deriv1(double arg) const {
    // TODO: Horner evaluation
    auto dhdt_00 = [](double t) { return 6*t*t - 6*t; };
    auto dhdt_10 = [](double t) { return 3*t*t - 4*t + 1; };
    auto dhdt_01 = [](double t) { return -6*t*t + 6*t; };
    auto dhdt_11 = [](double t) { return 3*t*t - 2*t; };

    index = find_interval(x, arg, index);
    double dx = x[index + 1] - x[index];
    double t = (arg - x[index])/dx;

    return dhdt_00(t)/dx*y[index] + dhdt_10(t)*m[index] + dhdt_01(t)/dx*y[index + 1] + dhdt_11(t)*m[index + 1];
}

double CubicSpline::deriv2(double arg) const {
    // TODO: Horner evaluation
    auto dhdt2_00 = [](double t) { return 12*t - 6; };
    auto dhdt2_10 = [](double t) { return 6*t - 4; };
    auto dhdt2_01 = [](double t) { return -12*t + 6; };
    auto dhdt2_11 = [](double t) { return 6*t - 2; };

    index = find_interval(x, arg, index);
    double dx = x[index + 1] - x[index];
    double t = (arg - x[index])/dx;

    return dhdt2_00(t)/(dx*dx)*y[index] + dhdt2_10(t)/dx*m[index] + dhdt2_01(t)/(dx*dx)*y[index + 1] + dhdt2_11(t)/dx*m[index + 1];
}

double CubicSpline::arg_min() const {
    return x.front();
}

double CubicSpline::arg_max() const {
    return x.back();
}
