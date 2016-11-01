#include "LinearSpline.hpp"

LinearSpline::LinearSpline(Series data)
    : data(data)
{
    // Check validity. Todo: Abstract this. Maybe as part of DataSeries.
    size_t n = data.size();
    for(size_t i = 0; i < n - 1; ++i)
    {
        if(data.arg(i) >= data.arg(i+1))
            throw std::runtime_error("Arguments have to be in ascending order");
    }
}

double LinearSpline::operator()(double arg) const
{
    size_t i = interval_index(arg);
    double p = (arg - data.arg(i))/(data.arg(i+1) - data.arg(i));

    return data.val(i)*(1.0 - p) + data.val(i+1)*p;
}

double LinearSpline::arg_min() const
{
    return data.args().front();
}

double LinearSpline::arg_max() const
{
    return data.args().back();
}

Series LinearSpline::sample(size_t n_points) const
{
    double arg0 = data.args().front();
    double arg1 = data.args().back();

    Series data;
    for(size_t i = 0; i <= n_points; ++i)
    {
        double alpha = double(i)/double(n_points);
        double arg = arg0*alpha + arg1*(1.0 - alpha);
        data.add(arg, (*this)(arg));
    }

    return data;
}

size_t LinearSpline::interval_index(double arg) const
{
    size_t lower = 0;
    size_t upper = data.args().size() - 1;

    while(upper - lower > 1)
    {
        size_t middle = (lower + upper)/2;

        if(data.arg(middle) < arg)
        {
            lower = middle;
        }
        else if(data.arg(middle) >= arg)
        {
            upper = middle;
        }
    }

    return lower;
}
