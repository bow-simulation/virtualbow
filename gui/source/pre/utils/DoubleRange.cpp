#include "DoubleRange.hpp"
#include <cmath>

DoubleBound::DoubleBound(double bound, bool inclusive):
    bound(bound),
    inclusive(inclusive)
{

}

bool DoubleBound::bounds_upper(double value) const {
    if(inclusive) {
        return value <= bound;
    } else {
        return value < bound;
    }
}

bool DoubleBound::bounds_lower(double value) const {
    if(inclusive) {
        return value >= bound;
    } else {
        return value > bound;
    }
}


DoubleRange::DoubleRange(std::optional<DoubleBound> lower, std::optional<DoubleBound> upper, double step):
    lower(lower),
    upper(upper),
    step(step)
{

}

bool DoubleRange::contains(double value) const {
    if(lower.has_value() && !lower->bounds_lower(value)) {
        return false;
    }

    if(upper.has_value() && !upper->bounds_upper(value)) {
        return false;
    }

    return true;
}

DoubleRange DoubleRange::inclusive(double min, double max, double step) {
    return DoubleRange(DoubleBound(min, true), DoubleBound(max, true), step);
}

DoubleRange DoubleRange::exclusive(double min, double max, double step) {
    return DoubleRange(DoubleBound(min, false), DoubleBound(max, false), step);
}

DoubleRange DoubleRange::largerOrEqual(double min, double step) {
    return DoubleRange(DoubleBound(min, true), std::nullopt, step);
}

DoubleRange DoubleRange::largerThan(double min, double step) {
    return DoubleRange(DoubleBound(min, false), std::nullopt, step);
}

DoubleRange DoubleRange::lessOrEqual(double max, double step) {
    return DoubleRange(std::nullopt, DoubleBound(max, true), step);
}

DoubleRange DoubleRange::lessThan(double max, double step) {
    return DoubleRange(std::nullopt, DoubleBound(max, false), step);
}

DoubleRange DoubleRange::unrestricted(double step) {
    return DoubleRange(std::nullopt, std::nullopt, step);
}

DoubleRange DoubleRange::positive(double step) {
    return DoubleRange(DoubleBound(0.0, false), std::nullopt, step);
}

DoubleRange DoubleRange::negative(double step) {
    return DoubleRange(std::nullopt, DoubleBound(0.0, true), step);
}

DoubleRange DoubleRange::nonPositive(double step) {
    return DoubleRange(std::nullopt, DoubleBound(0.0, true), step);
}

DoubleRange DoubleRange::nonNegative(double step) {
    return DoubleRange(DoubleBound(0.0, true), std::nullopt, step);
}
