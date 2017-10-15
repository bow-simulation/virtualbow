#pragma once
#include <vector>
#include <stdexcept>
#include <functional>

// Todo: Remove is_valid() and set_valid() from public interface
// Todo: Make depends_on(...) variadic

class DependentBase;

class IndependentBase
{
public:
    void add_dependent(DependentBase& dependent)
    {
        dependents.push_back(&dependent);
    }

protected:
    std::vector<DependentBase*> dependents;
};

class DependentBase: public IndependentBase
{
public:
    template<typename... Args>
    void depends_on(IndependentBase& other, Args&... args)
    {
        depends_on(other);
        depends_on(args...);
    }

    void depends_on(IndependentBase& other)
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
    }

protected:
    mutable bool valid = false;
};

template<typename T>
class Independent: public IndependentBase
{
public:
    Independent(const T& value)
        : value(value)
    {

    }

    Independent()
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
class Dependent: public DependentBase
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
        return value;
    }

private:
    F update;
    T value;
};
