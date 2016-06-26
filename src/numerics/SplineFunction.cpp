#include "SplineFunction.hpp"

bool SplineFunction::init(const std::vector<double>& args, const std::vector<double>& values)
{
    // Test validity of the input data
    if(args.size() != values.size() || args.size() < 2)
        return false;

    for(size_t i = 0; i < args.size() - 1; ++i)
    {
        if(args[i] >= args[i+1])
            return false;
    }

    size_t n = args.size();

    t = args;
    x = values;
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

    return true;
}

double SplineFunction::operator()(double arg) const
{
    size_t n = t.size();
    size_t i = interval_index(arg);
    double h = arg - t[i];

    return ((a[i]*h + b[i])*h + c[i])*h + x[i];
}

void SplineFunction::sample(std::vector<double>& args, std::vector<double>& values, size_t n_sample) const
{
    double arg0 = t.front();
    double arg1 = t.back();

    for(size_t i = 0; i <= n_sample; ++i)
    {
        double alpha = double(i)/double(n_sample);
        double arg = arg0*alpha + arg1*(1.0 - alpha);

        args.push_back(arg);
        values.push_back((*this)(arg));
    }
}

size_t SplineFunction::interval_index(double arg) const
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
