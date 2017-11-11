#pragma once

#include <string>
#include <boost/optional.hpp>

namespace validators
{
    template<typename T>
    static boost::optional<std::string> any(const T& value)
    {
        return boost::none;
    }

    template<typename T>
    static boost::optional<std::string> pos(const T& value)
    {
        if(value > 0)
            return boost::none;

        return std::string("Value must be positive");
    }

    template<typename T>
    static boost::optional<std::string> neg(const T& value)
    {
        if(value < 0)
            return boost::none;

        return std::string("Value must be negative");
    }

    template<typename T>
    static boost::optional<std::string> non_pos(const T& value)
    {
        if(value <= 0)
            return boost::none;

        return std::string("Value must not be positive");
    }

    template<typename T>
    static boost::optional<std::string> non_neg(const T& value)
    {
        if(value >= 0)
            return boost::none;

        return std::string("Value must not be negative");
    }
}
