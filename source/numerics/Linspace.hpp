#pragma once
#include <vector>
#include <cassert>

// Returns n linearly spaced samples from start to end
template<typename T>
std::vector<T> linspace(T start, T end, unsigned n)
{
    assert(n >= 2);

    std::vector<T> values(n);
    for(unsigned i = 0; i < n; ++i)
    {
        T delta = (end - start)/double(n - 1);
        values[i] = start + i*delta;
    }

    return values;
}

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

    // a: First value (inclusive)
    // b: Last value (inclusive)
    // n: Number of points
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

private:
    T a;
    T b;
    unsigned n;
};
