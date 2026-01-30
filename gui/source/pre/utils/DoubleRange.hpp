#pragma once
#include <optional>

// Interval bound that can be inclusive or exclusive
struct DoubleBound {
    double bound;
    bool inclusive;

    DoubleBound(double bound, bool inclusive);
    bool bounds_lower(double value) const;    // Whether the given value satisfies the bound as a lower bound
    bool bounds_upper(double value) const;    // Whether the given value satisfies the bound as an upper bound
};

// Range that consists of two optional interval bounds and a step
struct DoubleRange {
    std::optional<DoubleBound> lower;
    std::optional<DoubleBound> upper;
    double step;

    DoubleRange(std::optional<DoubleBound> lower, std::optional<DoubleBound> upper, double step);
    bool contains(double value) const;

    static DoubleRange inclusive(double min, double max, double step);
    static DoubleRange exclusive(double min, double max, double step);

    static DoubleRange largerOrEqual(double min, double step);
    static DoubleRange largerThan(double min, double step);
    static DoubleRange lessOrEqual(double max, double step);
    static DoubleRange lessThan(double max, double step);

    static DoubleRange unrestricted(double step);
    static DoubleRange positive(double step);
    static DoubleRange negative(double step);
    static DoubleRange nonPositive(double step);
    static DoubleRange nonNegative(double step);    
};
