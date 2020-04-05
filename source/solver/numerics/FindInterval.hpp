#pragma once
#include <vector>
#include <algorithm>
#include <cassert>

// Returns an index i such that x[i] <= value <= x[i+1].
// Requires x to be sorted and the value to lie within the first and last entry.
// Uses linear search starting with an optional initial guess.
static size_t find_interval(const std::vector<double>& x, double value, size_t start = 0)
{
    assert(x.size() >= 2 && std::is_sorted(x.begin(), x.end()));
    assert(value >= x.front() && value <= x.back());
    assert(start <= x.size() - 2);

    while(x[start + 1] < value) {
        ++start;
    }

    while(x[start] > value) {
        --start;
    }

    return start;
}
