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
