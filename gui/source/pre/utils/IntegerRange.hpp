#pragma once

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
