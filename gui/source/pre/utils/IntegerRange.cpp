#include "IntegerRange.hpp"
#include <limits>

IntegerRange IntegerRange::inclusive(int min, int max) {
    return {
        .min = min,
        .max = max
    };
}

IntegerRange IntegerRange::exclusive(int min, int max) {
    return {
        .min = min + 1,
        .max = max - 1
    };
}

IntegerRange IntegerRange::unrestricted() {
    return {
        .min = std::numeric_limits<int>::lowest(),
        .max = std::numeric_limits<int>::max()
    };
}

IntegerRange IntegerRange::positive() {
    return {
        .min = 1,
        .max = std::numeric_limits<int>::max()
    };
}

IntegerRange IntegerRange::negative() {
    return {
        .min = std::numeric_limits<int>::lowest(),
        .max = -1
    };
}

IntegerRange IntegerRange::nonPositive() {
    return {
        .min = std::numeric_limits<int>::lowest(),
        .max = 0
    };
}

IntegerRange IntegerRange::nonNegative() {
    return {
        .min = 0,
        .max = std::numeric_limits<int>::max()
    };
}
