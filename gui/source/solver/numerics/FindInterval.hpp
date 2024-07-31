#pragma once
#include "Utils.hpp"
#include "EigenTypes.hpp"
#include <vector>
#include <algorithm>
#include <cassert>

// More general version, takes in a vector of objects that provide the x values via a map function.
// Ideally there would be only one function that takes (a) double iterator(s), with the caller doing the mapping.
template<typename T, typename F>
inline size_t find_interval(const std::vector<T>& data, const F& map, double value, size_t start)
{
    assert(std::is_sorted(data.begin(), data.end(), [&](const T& a, const T& b){ return map(a) < map(b); }));

    start = clamp<size_t>(start, 0, data.size() - 1);
    while(start > 0 && map(data[start]) > value) {
        --start;
    }
    while(start < data.size()-2 && map(data[start + 1]) < value) {
        ++start;
    }

    return start;
}

// Returns an index i such that x[i] <= value <= x[i+1].
// Requires x to be sorted. If the value is out of bounds, the index of the first or last interval is returned, respectively.
// Uses a linear search starting with an optional initial guess.
inline size_t find_interval(const std::vector<double>& x, double value, size_t start = 0) {
    return find_interval(x, [](double x){ return x; }, value, start);
}

// Copypasta for VectorXd. Todo: Include this case in version above.
inline size_t find_interval(const VectorXd& data, double value, size_t start) {
    assert(std::is_sorted(data.begin(), data.end()));

    start = clamp<size_t>(start, 0, data.size() - 1);
    while(start > 0 && data[start] > value) {
        --start;
    }
    while(start < data.size()-2 && data[start + 1] < value) {
        ++start;
    }

    return start;
}
