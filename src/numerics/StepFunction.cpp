#include "StepFunction.hpp"

StepFunction::StepFunction(DataSeries data)
    : values(data.vals())
{
    // Check validity
    for(double w: data.args())
    {
        if(w <= 0)
            throw std::runtime_error("Arguments have to be positive");
    }

    // Successively sum up args to get interval points
    intervals.push_back(0.0);
    intervals.insert(intervals.end(), data.args().begin(), data.args().end());
    std::partial_sum(intervals.begin(), intervals.end(), intervals.begin());
}

double StepFunction::operator()(double arg) const
{
    return values[lower_index(arg)];
}

double StepFunction::arg_min() const
{
    return intervals.front();
}

double StepFunction::arg_max() const
{
    return intervals.back();
}
#include <QtCore>
DataSeries StepFunction::sample() const
{
    DataSeries data;

    size_t n = intervals.size();
    for(size_t i = 0; i < n; ++i)
    {
        if(i > 0)
        {
            data.add(intervals[i], values[i-1]);
        }

        if(i < n-1)
        {
            data.add(intervals[i], values[i]);
        }
    }

    return data;
}

// Todo: Abstract, or maybe somehow use std lower_bound
size_t StepFunction::lower_index(double arg) const
{
    size_t lower = 0;
    size_t upper = intervals.size() - 1;

    while(upper - lower > 1)
    {
        size_t middle = (lower + upper)/2;

        if(intervals[middle] < arg)
        {
            lower = middle;
        }
        else if(intervals[middle] >= arg)
        {
            upper = middle;
        }
    }

    return lower;
}
