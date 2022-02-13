#include "DoubleRange.hpp"
#include <limits>

DoubleRange DoubleRange::inclusive(double min, double max) {
    return {
        .min = min,
        .max = max
    };
}

DoubleRange DoubleRange::exclusive(double min, double max) {
    return {
        .min = min + std::numeric_limits<double>::min(),
        .max = max - std::numeric_limits<double>::min()
    };
}

DoubleRange DoubleRange::unrestricted() {
    return {
        .min = std::numeric_limits<double>::lowest(),
        .max = std::numeric_limits<double>::max()
    };
}

DoubleRange DoubleRange::positive() {
    return {
        .min = std::numeric_limits<double>::min(),
        .max = std::numeric_limits<double>::max()
    };
}

DoubleRange DoubleRange::negative() {
    return {
        .min = std::numeric_limits<double>::lowest(),
        .max = -std::numeric_limits<double>::min()
    };
}

DoubleRange DoubleRange::nonPositive() {
    return {
        .min = std::numeric_limits<double>::lowest(),
        .max = 0.0
    };
}

DoubleRange DoubleRange::nonNegative() {
    return {
        .min = 0.0,
        .max = std::numeric_limits<double>::max()
    };
}

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
