#pragma once

struct DoubleRange {
    double min;
    double max;

    static DoubleRange inclusive(double min, double max);
    static DoubleRange exclusive(double min, double max);

    static DoubleRange unrestricted();
    static DoubleRange positive();
    static DoubleRange negative();
    static DoubleRange nonPositive();
    static DoubleRange nonNegative();
};
