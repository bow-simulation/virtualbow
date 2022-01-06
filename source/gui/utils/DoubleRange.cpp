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
