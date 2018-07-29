#include "CubicSpline.hpp"

CubicSpline::CubicSpline(const Series& data)
    : xs(data.args()),
      ys(data.vals())
{
    // Check for minimum number of points
    if(data.size() < 2)
        throw std::runtime_error("at least two data points are needed");

    // Find sort permutation
    std::vector<size_t> p(xs.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&](size_t i, size_t j){ return xs[i] < xs[j]; });

    // Apply sort permutation
    std::transform(p.begin(), p.end(), xs.begin(), [&](size_t i){ return data.arg(i); });
    std::transform(p.begin(), p.end(), ys.begin(), [&](size_t i){ return data.val(i); });

    // Get consecutive differences and slopes
    std::vector<double> dys;
    std::vector<double> dxs;
    std::vector<double> ms;

    for(size_t i = 0; i < xs.size()-1; ++i)
    {
        double dx = xs[i+1] - xs[i];
        double dy = ys[i+1] - ys[i];

        if(dx == 0.0)
        {
            throw std::runtime_error("argument values must be unique");
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

double CubicSpline::operator()(double x, double y_default) const
{
    if(x < xs.front() || x > xs.back())
        return y_default;

    // Todo: Inefficient, use bisection
    size_t j = 0; // Last segment index
    auto eval_ascending = [&](double arg)
    {
        while(arg > xs[j+1])    // Advance segment index such that x[j] < arg < x[j + 1]
            ++j;

        double h = arg - xs[j];
        return ((c3s[j]*h + c2s[j])*h + c1s[j])*h + ys[j];
    };

    return eval_ascending(x);
}

// n: Number of intervals
Series CubicSpline::sample(size_t n)
{
    std::vector<double> args;
    args.resize(n+1);

    args[0] = xs.front();
    args[n] = xs.back();

    for(size_t i = 1; i < n; ++i)
    {
        double p = double(i)/double(n);
        args[i] = xs.front()*(1.0 - p) + xs.back()*p;
    }

    return Series(args, interpolate(args));
}

bool CubicSpline::is_strictly_increasing(const std::vector<double>& args)
{
    for(size_t i = 0; i < args.size()-1; ++i)
    {
        if(args[i] >= args[i+1])
            return false;
    }

    return true;
}

std::vector<double> CubicSpline::interpolate(const std::vector<double>& args)
{
    if(!is_strictly_increasing(args))
        throw std::runtime_error("function arguments must be strictly increasing");

    std::vector<double> vals;
    vals.resize(args.size());

    size_t j = 0; // Last segment index
    auto eval_ascending = [&](double arg)
    {
        while(j < vals.size()-1 && arg > xs[j+1])    // Advance segment index such that x[j] < arg < x[j + 1]
            ++j;

        double h = arg - xs[j];
        return ((c3s[j]*h + c2s[j])*h + c1s[j])*h + ys[j];
    };

    for(size_t i = 0; i < args.size(); ++i)
        vals[i] = eval_ascending(args[i]);

    return vals;
}
