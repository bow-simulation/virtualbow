#include "CubicSpline.hpp"
#include "solver/numerics/FindInterval.hpp"
#include "solver/numerics/Sorting.hpp"
#include <stdexcept>
#include <algorithm>
#include <numeric>

CubicSpline::CubicSpline(const std::vector<Vector<2>>& input) {
    std::vector<double> x;
    std::vector<double> y;

    // Extract x and y values from input, ignore rows that contain NaN
    for(auto& point: input) {
        if(!std::isnan(point(0)) && !std::isnan(point(1))) {
            x.push_back(point(0));
            y.push_back(point(1));
        }
    }

    if(x.size() < 2) {
        throw std::invalid_argument("At least two data points are needed");
    }
    if(x.size() != y.size()) {
        throw std::invalid_argument("Argument length mismatch");
    }

    // Sort inputs by arguments
    sort_by_argument(x, y);

    // Construct monotonic spline
    spline = tk::spline(x, y, tk::spline::cspline, true);
}

// Extrapolates on out of bounds access
double CubicSpline::operator()(double x) const {
    return spline(x);
}

// Returns default value on out of bounds access
double CubicSpline::operator()(double x, double y_default) const {
    if(x >= arg_min() && x <= arg_max()) {
        return operator()(x);
    }
    else {
        return y_default;
    }
}

double CubicSpline::arg_min() const {
    return spline.get_x_min();
}

double CubicSpline::arg_max() const {
    return spline.get_x_max();
}
