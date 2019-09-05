#pragma once
#include <vector>
#include <cassert>

// Class for lazy generation of linearly spaced values.
// n: Number of values
// a: First value
// b: Last value
template<typename T>
class Linspace
{
public:
    class Iterator
    {
    public:
        Iterator(const Linspace& linspace, unsigned index)
            : linspace(linspace), index(index)
        {

        }

        Iterator operator++()
        {
            ++index;
            return *this;
        }

        bool operator!=(const Iterator& other) const
        {
            return index != other.index;
        }

        T operator*() const
        {
            return linspace[index];
        }

    private:
       const Linspace& linspace;
       unsigned index;
    };

    Linspace(T a, T b, unsigned n)
        : a(a), b(b), n(n)
    {

    }
    unsigned size() const
    {
        return n;
    }

    T operator[](unsigned i) const
    {
        assert(i < n);
        return a + double(i)/(n-1)*(b - a);
    }

    Iterator begin() const
    {
        return Iterator(*this, 0);
    }

    Iterator end() const
    {
        return Iterator(*this, n);
    }

    std::vector<T> collect()
    {
        std::vector<T> result;
        result.reserve(n);
        for(const T& value: *this)
        {
            result.push_back(value);
        }

        return result;
    }

private:
    T a;
    T b;
    unsigned n;
};
