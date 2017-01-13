#pragma once
#include <vector>

// Comparison operators that compare all elements in the vector to a single value

template<typename T, typename U>
bool operator>=(const std::vector<T>& vector, U rhs)
{
    for(auto& value: vector)
    {
        if(value < rhs)
            return false;
    }

    return true;
}

template<typename T, typename U>
bool operator>(const std::vector<T>& vector, U rhs)
{
    for(auto& value: vector)
    {
        if(value <= rhs)
            return false;
    }

    return true;
}

template<typename T, typename U>
bool operator<=(const std::vector<T>& vector, U rhs)
{
    return !(vector > rhs);
}

template<typename T, typename U>
bool operator<(const std::vector<T>& vector, U rhs)
{
    return !(vector >= rhs);
}
