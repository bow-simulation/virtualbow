#pragma once

struct DoubleRange {
    double min;
    double max;
    double step;

    static const double DECIMALS;  // Number of decimals for tolerance
    static const double EPSILON;   // Tolerance for inclusive bounds

    static DoubleRange inclusive(double min, double max, double step);
    static DoubleRange exclusive(double min, double max, double step);

    static DoubleRange unrestricted(double step);
    static DoubleRange positive(double step);
    static DoubleRange negative(double step);
    static DoubleRange nonPositive(double step);
    static DoubleRange nonNegative(double step);
};
