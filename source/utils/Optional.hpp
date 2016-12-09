#pragma once
#include <stdexcept>
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
        if(valid)
            return data;
        else
            throw std::runtime_error("Bad Optional access");
    }

    MatrixType* operator->()
    {
        if(valid)
            return &data;
        else
            throw std::runtime_error("Bad Optional access");
    }

private:
    MatrixType data;
    bool valid;
};
