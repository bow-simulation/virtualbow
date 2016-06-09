#pragma once
#include "view_base.hpp"

#include <Eigen/Dense>

#include <exception>

using Eigen::VectorXd;
using Eigen::MatrixXd;

class Dof;

template<class T> class VectorAccess;
template<class T> class MatrixAccess;

template<class T>
using VectorView = VectorViewBase<VectorAccess<T>, Dof>;

template<class T>
using MatrixView = MatrixViewBase<MatrixAccess<T>, Dof>;

enum class DofType
{
    Active,
    Fixed,
};

struct Dof
{
    DofType type;
    size_t index;
};

// Todo: Avoid nullptr checks at runtime? (Template, different types, ...)
template<class T>
class VectorAccess
{
private:
    T* vec_active;
    T* vec_fixed;

public:
    VectorAccess(T* vec_active, T* vec_fixed)
        : vec_active(vec_active),
          vec_fixed(vec_fixed)
    {

    }

    double get(Dof dof) const
    {
        if(dof.type == DofType::Active && vec_active != nullptr)
        {
            return (*vec_active)(dof.index);
        }
        else if(dof.type == DofType::Fixed && vec_fixed != nullptr)
        {
            return (*vec_fixed)(dof.index);
        }
        else
        {
            return 0.0;
        }
    }

    void add(Dof dof, double val)
    {
        if(dof.type == DofType::Active && vec_active != nullptr)
        {
            (*vec_active)(dof.index) += val;
        }
        else if(dof.type == DofType::Fixed && vec_fixed != nullptr)
        {
            (*vec_fixed)(dof.index) += val;
        }
        else
        {
            throw std::runtime_error("Cannot add to this entry");
        }
    }
};

template<class T>
class MatrixAccess
{
private:
    T& mat;

public:
    MatrixAccess(T& mat)
        : mat(mat)
    {

    }

    double get(Dof dof_row, Dof dof_col) const
    {
        if(dof_row.type == DofType::Active && dof_col.type == DofType::Active)
        {
            return mat(dof_row.index, dof_col.index);
        }
    }

    void add(Dof dof_row, Dof dof_col, double val)
    {
        if(dof_row.type == DofType::Active && dof_col.type == DofType::Active)
        {
            mat(dof_row.index, dof_col.index) += val;
        }
    }
};
