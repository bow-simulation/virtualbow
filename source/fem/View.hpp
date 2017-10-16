#pragma once
#include "numerics/Math.hpp"
#include "Node.hpp"
#include <array>
#include <functional>

//================================================================================================

class LocalVectorEntry
{
public:
    std::function<double(Dof)>& get;
    std::function<void(Dof, double)>& set;
    std::function<void(Dof, double)>& add;
    Dof key;

    // Read
    operator double() const
    {
        return get(key);
    }

    // Write
    LocalVectorEntry& operator=(double rhs)
    {
        set(key, rhs);
        return *this;
    }

    void operator+=(double rhs)
    {
        add(key, rhs);
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }
};

template<size_t N>
class LocalVectorView
{
public:
    std::function<double(Dof)>& get;
    std::function<void(Dof, double)>& set;
    std::function<void(Dof, double)>& add;
    const std::array<Dof, N>& keys;

    // Read
    operator Vector<N>() const
    {
        Vector<N> res;
        for(size_t i = 0; i < N; ++i)
        {
            res(i) = get(keys[i]);
        }

        return res;
    }

    // Write
    template<class T>
    LocalVectorView<N>& operator=(const Eigen::MatrixBase<T>& rhs)
    {
        for(size_t i = 0; i < N; ++i)
        {
            set(keys[i], rhs(i));
        }

        return *this;
    }

    template<class T>
    void operator+=(const Eigen::MatrixBase<T>& rhs)
    {
        for(size_t i = 0; i < N; ++i)
        {
            add(keys[i], rhs(i));
        }
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }

    // Index
    LocalVectorEntry operator()(size_t i)
    {
        return LocalVectorEntry{get, set, add, keys[i]};
    }

    const LocalVectorEntry operator()(size_t i) const
    {
        return LocalVectorEntry{get, set, add, keys[i]};
    }
};

class VectorView
{
private:
    mutable std::function<double(Dof)> get;
    mutable std::function<void(Dof, double)> set;
    mutable std::function<void(Dof, double)> add;

public:
    VectorView(std::function<double(Dof)> get,
               std::function<void(Dof, double)> set,
               std::function<void(Dof, double)> add)
        : get(get),
          set(set),
          add(add)
    {

    }

    VectorView()
    {

    }

    // Multi index
    template<size_t N>
    LocalVectorView<N> operator()(const std::array<Dof, N>& keys)
    {
        return LocalVectorView<N>{get, set, add, keys};
    }

    template<size_t N>
    const LocalVectorView<N> operator()(const std::array<Dof, N>& keys) const
    {
        return LocalVectorView<N>{get, set, add, keys};
    }

    // Single indices
    LocalVectorEntry operator()(Dof key)
    {
        return LocalVectorEntry{get, set, add, key};
    }

    const LocalVectorEntry operator()(Dof key) const
    {
        return LocalVectorEntry{get, set, add, key};
    }
};

//================================================================================================

class LocalMatrixEntry
{
public:
    std::function<void(Dof, Dof, double)>& add;
    Dof key_row;
    Dof key_col;

    // Write symmetrically
    void operator+=(double rhs)
    {
        add(key_row, key_col, rhs);

        if(key_row != key_col)
        {
            add(key_col, key_row, rhs);
        }
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }
};


template<size_t N>
class LocalMatrixView
{
public:
    std::function<void(Dof, Dof, double)>& add;
    const std::array<Dof, N>& keys;

    // Write
    template<class T>
    void operator+=(const Eigen::MatrixBase<T>& rhs)
    {
        for(size_t j = 0; j < N; ++j)
        {
            for(size_t i = 0; i < N; ++i)
            {
                add(keys[i], keys[j], rhs(i, j));
            }
        }
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }

    // Index
    LocalMatrixEntry operator()(size_t i, size_t j)
    {
        return LocalMatrixEntry{add, keys[i], keys[j]};
    }

    const LocalMatrixEntry operator()(size_t i, size_t j) const
    {
        return LocalMatrixEntry{add, keys[i], keys[j]};
    }
};

class MatrixView
{
private:
    mutable std::function<void(Dof, Dof, double)> add;

public:
    MatrixView(std::function<void(Dof, Dof, double)> add)
        : add(add)
    {

    }

    MatrixView()
    {

    }

    // Index
    template<size_t N>
    LocalMatrixView<N> operator()(const std::array<Dof, N>& keys)
    {
        return LocalMatrixView<N>{add, keys};
    }

    template<size_t N>
    const LocalMatrixView<N> operator()(const std::array<Dof, N>& keys) const
    {
        return LocalMatrixView<N>{add, keys};
    }

    LocalMatrixEntry operator()(Dof key_row, Dof key_col)
    {
        return LocalMatrixEntry{add, key_row, key_col};
    }

    const LocalMatrixEntry operator()(Dof key_row, Dof key_col) const
    {
        return LocalMatrixEntry{add, key_row, key_col};
    }
};
