#include "LinearSpline.hpp"
#include "FindInterval.hpp"

LinearSpline::LinearSpline(const std::vector<double>& args, const std::vector<double>& vals)
    : x(args), y(vals) {

}

double LinearSpline::arg_min() const {
    return x.front();
}

double LinearSpline::arg_max() const {
    return x.back();
}

double LinearSpline::operator()(double arg) const {
    index = find_interval(x, arg, index);
    double dx = x[index + 1] - x[index];
    double t = (arg - x[index])/dx;
    return y[index]*(1.0 - t) + y[index + 1]*t;
}

double LinearSpline::operator()(double arg, double default_value) const {
    if(arg >= x.front() && arg <= x.back()) {
        return operator()(arg);
    } else {
        return default_value;
    }
}
