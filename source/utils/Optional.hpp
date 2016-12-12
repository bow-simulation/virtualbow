#pragma once
#include <cassert>
#include <Eigen/Core>

template<class MatrixType>
class Optional
{
public:
    Optional(void)
        : valid(false)
    {

    }

    bool is_valid() const
    {
        return valid;
    }

    void set_valid(bool v)
    {
        valid = v;
    }


    MatrixType& operator*()
    {
        assert(valid);
        return data;
    }

    MatrixType* operator->()
    {
        assert(valid);
        return &data;
    }

private:
    MatrixType data;
    bool valid;
};
