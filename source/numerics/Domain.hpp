#pragma once

template<typename T>
class Domain
{
public:
    static bool pos(const T& value)
    {
        return value > 0;
    }

    static bool neg(const T& value)
    {
        return value < 0;
    }

    static bool non_pos(const T& value)
    {
        return value <= 0;
    }

    static bool non_neg(const T& value)
    {
        return value >= 0;
    }
};
