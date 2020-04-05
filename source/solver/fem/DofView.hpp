#pragma once
#include "Node.hpp"
#include "solver/numerics/Eigen.hpp"
#include <array>

static double get_by_dof(const VectorXd* vec_a, const VectorXd* vec_f, Dof dof)
{
    if(vec_a && dof.active)
        return (*vec_a)[dof.index];

    if(vec_f && !dof.active)
        return (*vec_f)[dof.index];

    return 0.0;
}

static void set_by_dof(VectorXd* vec_a, VectorXd* vec_f, Dof dof, double value)
{
    if(vec_a && dof.active) {
        (*vec_a)[dof.index] = value;
    }
    else if (vec_f && !dof.active) {
        (*vec_f)[dof.index] = value;
    }
}

static void add_by_dof(VectorXd* vec_a, VectorXd* vec_f, Dof dof, double value)
{
    if(vec_a && dof.active) {
        (*vec_a)[dof.index] += value;
    }
    else if (vec_f && !dof.active) {
        (*vec_f)[dof.index] += value;
    }
}

template<size_t N>
static Vector<N> get_by_dof(const VectorXd* vec_a, const VectorXd* vec_f, const std::array<Dof, N> dofs)
{
    Vector<N> vec;
    for(size_t i = 0; i < N; ++i)
        vec[i] = get_by_dof(vec_a, vec_f, dofs[i]);

    return vec;
}

template<size_t N, class T>
static void set_by_dof(VectorXd* vec_a, VectorXd* vec_f, const std::array<Dof, N> dofs, const T& values)
{
    for(size_t i = 0; i < N; ++i)
        set_by_dof(vec_a, vec_f, dofs[i], values[i]);
}

template<size_t N, class T>
static void add_by_dof(VectorXd* vec_a, VectorXd* vec_f, const std::array<Dof, N> dofs, const T& values)
{
    for(size_t i = 0; i < N; ++i)
        add_by_dof(vec_a, vec_f, dofs[i], values[i]);
}

template<size_t N, class T>
static void add_by_dof(MatrixXd* mat, const std::array<Dof, N> dofs, const T& values)
{
    for(size_t i = 0; i < N; ++i)
    {
        for(size_t j = 0; j < N; ++j)
        {
            if(dofs[i].active && dofs[j].active)
                (*mat)(dofs[i].index, dofs[j].index) += values(i, j);
        }
    }
}
