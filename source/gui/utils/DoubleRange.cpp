#include "DoubleRange.hpp"
#include <limits>
#include <cmath>

const double DoubleRange::DECIMALS = 8;      // Magic number
const double DoubleRange::EPSILON = std::pow(10, -DoubleRange::DECIMALS);

DoubleRange DoubleRange::inclusive(double min, double max, double step) {
    return {
        .min = min,
        .max = max,
        .step = step
    };
}

DoubleRange DoubleRange::exclusive(double min, double max, double step) {
    return {
        .min = min + EPSILON,
        .max = max - EPSILON,
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
        .min = EPSILON,
        .max = std::numeric_limits<double>::max(),
        .step = step
    };
}

DoubleRange DoubleRange::negative(double step) {
    return {
        .min = std::numeric_limits<double>::lowest(),
        .max = -EPSILON,
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
