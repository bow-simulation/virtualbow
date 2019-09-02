#pragma once
#include <vector>
#include <numeric>
#include <algorithm>

// Sorts the vector args in ascending order and applies the same permutation to vals
static void sort_by_argument(std::vector<double>& args, std::vector<double>& vals)
{
    std::vector<double> old_args = args;
    std::vector<double> old_vals = vals;

    // Find sort permutation
    std::vector<size_t> p(args.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&](size_t i, size_t j){ return args[i] < args[j]; });

    // Apply sort permutation
    std::transform(p.begin(), p.end(), args.begin(), [&](size_t i){ return old_args[i]; });
    std::transform(p.begin(), p.end(), vals.begin(), [&](size_t i){ return old_vals[i]; });
}
