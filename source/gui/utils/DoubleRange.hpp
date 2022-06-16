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

// TODO: Unify with DoubleRange and make templated
struct IntegerRange {
    int min;
    int max;

    static IntegerRange inclusive(int min, int max);
    static IntegerRange exclusive(int min, int max);

    static IntegerRange unrestricted();
    static IntegerRange positive();
    static IntegerRange negative();
    static IntegerRange nonPositive();
    static IntegerRange nonNegative();
};
