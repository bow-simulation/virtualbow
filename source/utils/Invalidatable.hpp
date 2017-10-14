#pragma once

template<typename T>
class Invalidatable
{
public:
    Invalidatable()
        : valid(false)
    {

    }

    bool is_valid() const
    {
        return valid;
    }

    void invalidate()
    {
        valid = false;
    }

    void set_valid()
    {
        valid = true;
    }

    T& operator*()
    {
        return value;
    }

    T* operator->()
    {
        return &value;
    }

private:
    T value;
    bool valid;
};
