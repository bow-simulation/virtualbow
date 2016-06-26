#include "StepFunction.hpp"

StepFunction::StepFunction(std::vector<double> lengths, std::vector<double> values)
    : values(values)
{
    assert(lengths.size() == values.size());

    // Successively sum up lengths to get interval points
    intervals.push_back(0.0);
    intervals.insert(intervals.end(), lengths.begin(), lengths.end());
    std::partial_sum(intervals.begin(), intervals.end(), intervals.begin());
}

double StepFunction::operator()(double arg) const
{
    return values[lower_index(arg)];
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
