#pragma once
#include "Utils.hpp"
#include <vector>
#include <algorithm>
#include <cassert>

// Returns an index i such that x[i] <= value <= x[i+1].
// Requires x to be sorted. If the value is out of bounds, the index of the first or last interval is returned, respectively.
// Uses a linear search starting with an optional initial guess.
static size_t find_interval(const std::vector<double>& x, double value, size_t start = 0)
{
    assert(std::is_sorted(x.begin(), x.end()));
    assert(value >= x.front() && value <= x.back());

    start = clamp<size_t>(start, 0, x.size() - 1);

    while(start > 0 && x[start] > value) {
        --start;
    }

    while(start < x.size()-2 && x[start + 1] < value) {
        ++start;
    }

    return start;
}
