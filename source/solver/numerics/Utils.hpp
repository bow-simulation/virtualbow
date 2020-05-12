#pragma once
#include <algorithm>

// https://stackoverflow.com/a/4609795
template<typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

// Clamps x to a value between a and b.
// Which one of a and b is the upper and lower bound is not relevant.
template<typename T>
const T& clamp(const T& x, const T& a, const T& b)
{
    const T& lower = std::min(a, b);
    const T& upper = std::max(a, b);

    if(x < lower) {
        return lower;
    }

    if(x > upper) {
        return upper;
    }

    return x;
};
