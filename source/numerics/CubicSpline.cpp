#include "CubicSpline.hpp"

CubicSpline::CubicSpline(Series data)
{
    // Check validity
    size_t n = data.size();
    for(size_t i = 0; i < n - 1; ++i)
    {
        if(data.arg(i) >= data.arg(i+1))
            throw std::runtime_error("Arguments have to be in ascending order");
    }

    t = data.args();    // Todo: Maybe save the input data instead of doing these copies here?
    x = data.vals();
    a.resize(n);
    b.resize(n);
    c.resize(n);

    TDMatrix<double> A(n);
    for(size_t i  = 1; i < n-1; ++i)
    {
        A.set(i, i-1, 1./3.*(t[i] - t[i-1]));
        A.set(i, i  , 2./3.*(t[i+1] - t[i-1]));
        A.set(i, i+1, 1./3.*(t[i+1] - t[i]));
        b[i] = (x[i+1] - x[i])/(t[i+1] - t[i]) - (x[i] - x[i-1])/(t[i] - t[i-1]);
    }

    A.set(0, 0, 2.0);
    A.set(0, 1, 0.0);
    A.set(n-1, n-1, 2.0);
    A.set(n-1, n-2, 0.0);
    b[0] = x[0] - x[0];     // Ugly hack to get type-independent zero
    b[n-1] = x[0] - x[0];   // Ugly hack

    A.solve(b);    // b was rhs, is now solution to A*b = rhs

    for(size_t i = 0; i < n-1; ++i)
    {
        double delta_t = t[i+1] - t[i];
        double delta_x = x[i+1] - x[i];
        a[i] = 1.0/3.0*(b[i+1] - b[i])/delta_t;
        c[i] = delta_x/delta_t - 1./3.*(2*b[i] + b[i+1])*delta_t;
    }
}

double CubicSpline::operator()(double arg) const
{
    size_t i = interval_index(arg);
    double h = arg - t[i];

    return ((a[i]*h + b[i])*h + c[i])*h + x[i];
}

double CubicSpline::arg_min() const
{
    return t.front();
}

double CubicSpline::arg_max() const
{
    return t.back();
}

Series CubicSpline::sample(size_t n_points) const
{
    double arg0 = t.front();
    double arg1 = t.back();

    Series data;
    for(size_t i = 0; i <= n_points; ++i)
    {
        double alpha = double(i)/double(n_points);
        double arg = arg0*alpha + arg1*(1.0 - alpha);

        data.add(arg, (*this)(arg));
    }

    return data;
}

size_t CubicSpline::interval_index(double arg) const
{
    size_t lower = 0;
    size_t upper = t.size() - 1;

    while(upper - lower > 1)
    {
        size_t middle = (lower + upper)/2;

        if(t[middle] < arg)
        {
            lower = middle;
        }
        else if(t[middle] >= arg)
        {
            upper = middle;
        }
    }

    return lower;
}
