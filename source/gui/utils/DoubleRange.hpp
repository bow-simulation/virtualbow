#pragma once

struct DoubleRange {
    double min;
    double max;
    double step;

    static DoubleRange inclusive(double min, double max, double step);
    static DoubleRange exclusive(double min, double max, double step);

    static DoubleRange unrestricted(double step);
    static DoubleRange positive(double step);
    static DoubleRange negative(double step);
    static DoubleRange nonPositive(double step);
    static DoubleRange nonNegative(double step);
};
