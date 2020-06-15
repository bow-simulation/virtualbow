#include "CubicSpline.hpp"
#include "solver/numerics/FindInterval.hpp"
#include "solver/numerics/Sorting.hpp"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>

CubicSpline::CubicSpline(const MatrixXd& input)
{
    if(input.cols() != 2)
        throw std::invalid_argument("Input must have two columns");

    // Extract x and y values from input, ignore rows that contain NaN
    for(int i = 0; i < input.rows(); ++i)
    {
        double x = input(i, 0);
        double y = input(i, 1);
        if(!std::isnan(x) && !std::isnan(y)) {
            xs.push_back(x);
            ys.push_back(y);
        }
    }

    if(xs.size() < 2)
        throw std::invalid_argument("Input must contain at least two valid rows");

    // Sort inputs
    sort_by_argument(xs, ys);

    // Get consecutive differences and slopes
    std::vector<double> dys;
    std::vector<double> dxs;
    std::vector<double> ms;

    for(size_t i = 0; i < xs.size()-1; ++i)
    {
        double dx = xs[i+1] - xs[i];
        double dy = ys[i+1] - ys[i];

        if(dx == 0.0) {
            throw std::invalid_argument("Argument values must be unique");
        }

        dxs.push_back(dx);
        dys.push_back(dy);
        ms.push_back(dy/dx);
    }

    // Get degree-1 coefficients
    c1s.push_back(ms[0]);
    for(size_t i = 0; i < dxs.size() - 1; ++i)
    {
        if (ms[i]*ms[i+1] <= 0.0)
        {
            c1s.push_back(0.0);
        }
        else
        {
            double common = dxs[i] + dxs[i+1];
            c1s.push_back(3*common/((common + dxs[i+1])/ms[i] + (common + dxs[i])/ms[i+1]));
        }
    }
    c1s.push_back(ms.back());

    // Get degree-2 and degree-3 coefficients
    for(size_t i = 0; i < c1s.size() - 1; ++i)
    {
        double common = c1s[i] + c1s[i+1] - 2.0*ms[i];
        c2s.push_back((ms[i] - c1s[i] - common)/dxs[i]);
        c3s.push_back(common/(dxs[i]*dxs[i]));
    }
}

// Extrapolates on out of bounds access
double CubicSpline::operator()(double x) const
{
    index = find_interval(xs, x, index);
    double h = x - xs[index];
    return ((c3s[index]*h + c2s[index])*h + c1s[index])*h + ys[index];
}

// Returns default value on out of bounds access
double CubicSpline::operator()(double x, double y_default) const
{
    if(x >= xs.front() && x <= xs.back())
        return operator()(x);
    else
        return y_default;
}

double CubicSpline::arg_min() const
{
    return xs.front();
}

double CubicSpline::arg_max() const
{
    return xs.back();
}
