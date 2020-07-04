#include "CubicSpline.hpp"
#include "solver/numerics/FindInterval.hpp"
#include "solver/numerics/Sorting.hpp"
#include "solver/numerics/Utils.hpp"
#include <nlopt.hpp>
#include <stdexcept>
#include <cmath>

CubicSpline::CubicSpline(const MatrixXd& input)
{
    if(input.cols() != 2) {
        throw std::invalid_argument("Input must have two columns");
    }

    // Extract x and y values from input, ignore rows that contain NaN
    for(int i = 0; i < input.rows(); ++i)
    {
        if(!std::isnan(input(i, 0)) && !std::isnan(input(i, 1))) {
            x.push_back(input(i, 0));
            y.push_back(input(i, 1));
            m.push_back(0.0);    // Initial values, later determined by optimization
        }
    }

    if(x.size() < 2) {
        throw std::invalid_argument("Input must contain at least two valid rows");
    }

    // Sort inputs, check for duplicates
    sort_by_argument(x, y);
    for(size_t i = 0; i < x.size()-1; ++i) {
        if(!(x[i] < x[i+1]))
            throw std::invalid_argument("Argument values must be distinct");
    }

    auto objective = [](const std::vector<double>& m, std::vector<double>& grad, void* data)
    {
        if(!grad.empty()) {
            throw std::invalid_argument("This function does not compute gradients");
        }

        auto spline = static_cast<CubicSpline*>(data);

        double result = 0.0;
        for(size_t i = 0; i < m.size()-1; ++i)
        {
            double dx = spline->x[i+1] - spline->x[i];
            double dy = spline->y[i+1] - spline->y[i];

            result += (12.0*dy*dy - 12.0*(m[i+1] + m[i])*dx*dy + 4.0*(m[i+1]*m[i+1] + m[i]*m[i+1] + m[i]*m[i])*dx*dx)/(dx*dx*dx);
        }
        return result;
    };

    struct ConstraintData {
        size_t index;
        enum {IE1, IE2, IE3, IE4, IE5, IE6, EQ1, EQ2} type;
        CubicSpline* spline;
    };

    auto constraint = [](const std::vector<double>& m, std::vector<double>& grad, void *data)
    {
        if(!grad.empty()) {
            throw std::invalid_argument("This function does not compute gradients");
        }

        auto cdata = static_cast<ConstraintData*>(data);

        size_t i = cdata->index;
        auto& x = cdata->spline->x;
        auto& y = cdata->spline->y;

        double ms = (y[i+1] - y[i])/(x[i+1] - x[i]);
        double m0 = m[cdata->index];
        double m1 = m[cdata->index + 1];

        switch(cdata->type)
        {
        case ConstraintData::IE1:
            return -m0*sgn(ms);
        case ConstraintData::IE2:
            return -m1*sgn(ms);
        case ConstraintData::IE3:
            return (m0 - m1)*sgn(ms) - 3*std::abs(ms);
        case ConstraintData::IE4:
            return (m1 - m0)*sgn(ms) - 3*std::abs(ms);
        case ConstraintData::IE5:
            return (2*m0 + m1)*sgn(ms) - 9*std::abs(ms);
        case ConstraintData::IE6:
            return (m0 + 2*m1)*sgn(ms) - 9*std::abs(ms);
        case ConstraintData::EQ1:
            return m0;
        case ConstraintData::EQ2:
            return m1;
        }
    };

    // Magic numbers
    const double ftol = 1e-6;    // Relative tolerance on function value
    const double ctol = 1e-8;    // Tolerance on constraints
    const double iter = 150;     // Maximum number of function evaluations

    // Set up algorithm and objetive
    nlopt::opt opt(nlopt::algorithm::LN_COBYLA, m.size());
    opt.set_min_objective(objective, this);
    opt.set_xtol_rel(ftol);    // Magic number
    opt.set_maxeval(iter);

    // Set up constraints
    std::vector<ConstraintData> cdata;
    cdata.reserve(6*(m.size()-1));
    for(size_t i = 0; i < m.size()-1; ++i)
    {
        // Add six inequality constraints or two equality constraints depending on the type of interval
        if(y[i] != y[i+1])
        {
            cdata.push_back({.index = i, .type = ConstraintData::IE1, .spline = this });
            opt.add_inequality_constraint(constraint, &cdata.back(), ctol);

            cdata.push_back({.index = i, .type = ConstraintData::IE2, .spline = this });
            opt.add_inequality_constraint(constraint, &cdata.back(), ctol);

            cdata.push_back({.index = i, .type = ConstraintData::IE3, .spline = this });
            opt.add_inequality_constraint(constraint, &cdata.back(), ctol);

            cdata.push_back({.index = i, .type = ConstraintData::IE4, .spline = this });
            opt.add_inequality_constraint(constraint, &cdata.back(), ctol);

            cdata.push_back({.index = i, .type = ConstraintData::IE5, .spline = this });
            opt.add_inequality_constraint(constraint, &cdata.back(), ctol);

            cdata.push_back({.index = i, .type = ConstraintData::IE6, .spline = this });
            opt.add_inequality_constraint(constraint, &cdata.back(), ctol);
        }
        else
        {
            cdata.push_back({.index = i, .type = ConstraintData::EQ1, .spline = this });
            opt.add_equality_constraint(constraint, &cdata.back(), ctol);

            cdata.push_back({.index = i, .type = ConstraintData::EQ2, .spline = this });
            opt.add_equality_constraint(constraint, &cdata.back(), ctol);
        }
    }

    // Perform optimization
    double f_min;
    if(opt.optimize(m, f_min) < 0) {
        throw std::runtime_error("Failed to determine cubic spline");
    }
}

// Extrapolates on out of bounds access
double CubicSpline::operator()(double arg) const
{
    auto h00 = [](double t) { return 2*t*t*t - 3*t*t + 1; };
    auto h10 = [](double t) { return t*t*t - 2*t*t + t; };
    auto h01 = [](double t) { return -2*t*t*t + 3*t*t; };
    auto h11 = [](double t) { return t*t*t - t*t; };

    index = find_interval(x, arg, index);
    double dx = x[index + 1] - x[index];
    double t = (arg - x[index])/dx;

    return h00(t)*y[index] + h10(t)*m[index]*dx + h01(t)*y[index + 1] + h11(t)*m[index + 1]*dx;
}

// Returns default value on out of bounds access
double CubicSpline::operator()(double arg, double default_value) const
{
    if(arg >= x.front() && arg <= x.back()) {
        return operator()(arg);
    } else {
        return default_value;
    }
}

double CubicSpline::arg_min() const
{
    return x.front();
}

double CubicSpline::arg_max() const
{
    return x.back();
}
