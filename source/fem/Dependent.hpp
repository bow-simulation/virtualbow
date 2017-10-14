#pragma once
#include <vector>
#include <stdexcept>

class DependentBase
{
public:
    void depends_on(DependentBase& other)
    {
        other.dependents.push_back(this);
    }

    bool is_valid() const
    {
        return valid;
    }

    void set_valid(bool v)
    {
        valid = v;
    }

protected:
    std::vector<DependentBase*> dependents;
    bool valid = false;
};

template<typename T>
class Dependent: public DependentBase
{
public:
    Dependent()
    {

    }

    Dependent(const T& value)
        : value(value)
    {
        set_valid(true);
    }

    const T& get() const
    {
        if(!is_valid())
            throw std::runtime_error("Trying to access invalid value");

        return value;
    }

    T& mut()
    {
        for(auto dep: dependents)
            dep->set_valid(false);

        set_valid(true);
        return value;
    }

private:
    T value;
}; 
