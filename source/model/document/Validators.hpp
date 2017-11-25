#pragma once

namespace validators
{
    template<typename T>
    static bool any(const T& x)
    {
        return true;
    }

    template<typename T>
    static bool pos(const T& x)
    {
        return x > 0;
    }

    template<typename T>
    static bool neg(const T& x)
    {
        return x < 0;
    }

    template<typename T>
    static bool non_pos(const T& x)
    {
        return x <= 0;
    }

    template<typename T>
    static bool non_neg(const T& x)
    {
        return x >= 0;
    }
}
