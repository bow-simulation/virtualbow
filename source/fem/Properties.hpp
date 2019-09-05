#pragma once
#include <vector>
#include <stdexcept>
#include <functional>

// Properties and Bindings model the relationship between independent states (properties)
// and dependent computed values (bindings). Bindings compute and cache their value from
// their assigned properties when accessed. Changes to those properties invalidate the
// cached values of the dependent bindings. Their values are lazily recomputed on the next access.
//
// Naming inspired by JavaFX properties and bindings system.
// Todo: Remove is_valid() and set_valid() from public interface

class BindingBase;

class PropertyBase
{
public:
    void add_dependent(BindingBase& dependent)
    {
        dependents.push_back(&dependent);
    }

protected:
    std::vector<BindingBase*> dependents;
};

class BindingBase: public PropertyBase
{
public:
    template<typename... Args>
    void depends_on(PropertyBase& other, Args&... args)
    {
        depends_on(other);
        depends_on(args...);
    }

    void depends_on(PropertyBase& other)
    {
        other.add_dependent(*this);
    }

    bool is_valid() const
    {
        return valid;
    }

    void set_valid(bool val) const
    {
        valid = val;

        if(!valid)
        {
            for(auto d: dependents)
                d->set_valid(false);
        }
    }

protected:
    mutable bool valid = false;
};

template<typename T>
class Property: public PropertyBase
{
public:
    Property(const T& value)
        : value(value)
    {

    }

    Property()
    {

    }

    const T& get() const
    {
        return value;
    }

    T& mut()
    {
        for(auto d: dependents)
            d->set_valid(false);

        return value;
    }

private:
    T value;
};

template<typename T>
class Binding: public BindingBase
{
public:
    using F = std::function<void(void)>;

    void on_update(const F& f)
    {
        update = f;
    }

    const T& get() const
    {
        if(!is_valid())
        {
            update();
            set_valid(true);
        }

        return value;
    }

    T& mut()
    {
        for(auto d: dependents)
            d->set_valid(false);

        return value;
    }

private:
    F update;
    T value;
};
