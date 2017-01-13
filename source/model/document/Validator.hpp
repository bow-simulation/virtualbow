#pragma once
#include <functional>

#include "../../numerics/StdVector.hpp" // Todo: Remove

template<typename T>
class Validator
{
public:
    Validator(std::function<bool(const T&)> is_valid)
        : is_valid(is_valid)
    {

    }

    bool operator()(const T& value)
    {
        return is_valid(value);
    }

    Validator<T> operator&&(const Validator<T>& rhs)
    {
        return Validator<T>([=](const T& value)
        {
            return is_valid(value) && rhs.is_valid(value);
        });
    }

    Validator<T> operator!()
    {
        return Validator<T>([=](const T& value)
        {
            return !is_valid(value);
        });
    }

private:
    std::function<bool(const T&)> is_valid;
};

template<typename T>
static Validator<T> All()
{
    return Validator<T>([](const T& value)
    {
        return true;
    });
}

template<typename T, typename U>
static Validator<T> InclusiveRange(U min, U max)
{
    return Validator<T>([=](const T& value)
    {
        return value >= min && value <= max;
    });
}

template<typename T, typename U>
static Validator<T> ExclusiveRange(U min, U max)
{
    return Validator<T>([=](const T& value)
    {
        return value > min && value < max;
    });
}

template<typename T>
static Validator<T> Positive()
{
    return Validator<T>([](const T& value)
    {
        return value > 0;
    });
}

template<typename T>
static Validator<T> Negative()
{
    return Validator<T>([](const T& value)
    {
        return value < 0;
    });
}

// Container-specific

template<typename T>
static Validator<T> Ascending()
{
    return Validator<T>([](const T& value)
    {
        for(size_t i = 1; i < value.size(); ++i)
        {
            if(value[i-1] >= value[i])
                return false;
        }

        return true;
    });
}
