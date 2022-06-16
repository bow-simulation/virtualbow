#include "DoubleRange.hpp"
#include <limits>

DoubleRange DoubleRange::inclusive(double min, double max, double step) {
    return {
        .min = min,
        .max = max,
        .step = step
    };
}

DoubleRange DoubleRange::exclusive(double min, double max, double step) {
    return {
        .min = min + std::numeric_limits<double>::min(),
        .max = max - std::numeric_limits<double>::min(),
        .step = step
    };
}

DoubleRange DoubleRange::unrestricted(double step) {
    return {
        .min = std::numeric_limits<double>::lowest(),
        .max = std::numeric_limits<double>::max(),
        .step = step
    };
}

DoubleRange DoubleRange::positive(double step) {
    return {
        .min = std::numeric_limits<double>::min(),
        .max = std::numeric_limits<double>::max(),
        .step = step
    };
}

DoubleRange DoubleRange::negative(double step) {
    return {
        .min = std::numeric_limits<double>::lowest(),
        .max = -std::numeric_limits<double>::min(),
        .step = step
    };
}

DoubleRange DoubleRange::nonPositive(double step) {
    return {
        .min = std::numeric_limits<double>::lowest(),
        .max = 0.0,
        .step = step
    };
}

DoubleRange DoubleRange::nonNegative(double step) {
    return {
        .min = 0.0,
        .max = std::numeric_limits<double>::max(),
        .step = step
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
