#pragma once
#include <Eigen/Core>

#include <array>

template<std::size_t n>
using Matrix = Eigen::Matrix<double, n, n>;

template<std::size_t n>
using Vector = Eigen::Matrix<double, n, 1>;

/*
Access interface for VectorView:
    double get(Key) const
    void add(Key, double)
*/

template<class Access, class Key>
class LocalVectorEntry
{
private:
    Access access;
    Key key;

public:
    LocalVectorEntry(Access access, Key key)
        : access(access), key(key)
    {

    }

    // Read
    operator double() const
    {
        return access.get(key);
    }

    // Write
    void operator+=(double rhs)
    {
        access.add(key, rhs);
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }
};

template<class Access, class Key, size_t N>
class LocalVectorView
{
private:
    Access access;
    const std::array<Key, N>& keys;

public:
    LocalVectorView(Access access, const std::array<Key, N>& keys)
        : access(access), keys(keys)
    {

    }

    // Read
    operator Vector<N>() const
    {
        Vector<N> res;
        for(size_t i = 0; i < N; ++i)
        {
            res(i) = access.get(keys[i]);
        }

        return res;
    }

    // Write
    template<class T>
    void operator+=(const Eigen::MatrixBase<T>& rhs)
    {
        for(size_t i = 0; i < N; ++i)
        {
            access.add(keys[i], rhs(i));
        }
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }

    // Index
    LocalVectorEntry<Access, Key> operator()(size_t i)
    {
        return LocalVectorEntry<Access, Key>(access, keys[i]);
    }

    const LocalVectorEntry<Access, Key> operator()(size_t i) const
    {
        return LocalVectorEntry<Access, Key>(access, keys[i]);
    }
};

template<class Access, class Key>
class VectorViewBase
{
private:
    Access access;

public:
    VectorViewBase(Access access)
        : access(access)
    {

    }

    // Index
    template<size_t N>
    LocalVectorView<Access, Key, N> operator()(const std::array<Key, N>& keys)
    {
        return LocalVectorView<Access, Key, N>(access, keys);
    }

    template<size_t N>
    const LocalVectorView<Access, Key, N> operator()(const std::array<Key, N>& keys) const
    {
        return LocalVectorView<Access, Key, N>(access, keys);
    }

    LocalVectorEntry<Access, Key> operator()(Key key)
    {
        return LocalVectorEntry<Access, Key>(access, key);
    }

    const LocalVectorEntry<Access, Key> operator()(Key key) const
    {
        return LocalVectorEntry<Access, Key>(access, key);
    }
};

/*
Access interface for MatrixView:
    double get(Key, Key) const
    void add(Key, Key, double)
*/

template<class Access, class Key>
class LocalMatrixEntry
{
private:
    Access access;
    Key key_row;
    Key key_col;

public:
    LocalMatrixEntry(Access access, Key key_row, Key key_col)
        : access(access), key_row(key_row), key_col(key_col)
    {

    }

    // Read
    operator double() const
    {
        return access.get(key_row, key_col);
    }

    // Write
    void operator+=(double rhs)
    {
        access.add(key_row, key_col, rhs);
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }
};

template<class Access, class Key, size_t N>
class LocalMatrixView
{
private:
    Access access;
    const std::array<Key, N>& keys;

public:
    LocalMatrixView(Access access, const std::array<Key, N>& keys)
        : access(access), keys(keys)
    {

    }

    // Read
    operator Matrix<N>() const
    {
        Matrix<N> res;
        for(size_t j = 0; j < N; ++j)
        {
            for(size_t i = 0; i < N; ++i)
            {
                res(i, j) = access.get(keys[i], keys[j]);
            }
        }

        return res;
    }

    // Write
    template<class T>
    void operator+=(const Eigen::MatrixBase<T>& rhs)
    {
        for(size_t j = 0; j < N; ++j)
        {
            for(size_t i = 0; i < N; ++i)
            {
                access.add(keys[i], keys[j], rhs(i, j));
            }
        }
    }

    void operator-=(double rhs)
    {
        operator+=(-rhs);
    }

    // Index
    LocalMatrixEntry<Access, Key> operator()(size_t i, size_t j)
    {
        return LocalMatrixEntry<Access, Key>(access, keys[i], keys[j]);
    }

    const LocalMatrixEntry<Access, Key> operator()(size_t i, size_t j) const
    {
        return LocalMatrixEntry<Access, Key>(access, keys[i], keys[j]);
    }
};

template<class Access, class Key>
class MatrixViewBase
{
private:
    Access access;

public:
    MatrixViewBase(Access access)
        : access(access)
    {

    }

    // Index
    template<size_t N>
    LocalMatrixView<Access, Key, N> operator()(const std::array<Key, N>& keys)
    {
        return LocalMatrixView<Access, Key, N>(access, keys);
    }

    template<size_t N>
    const LocalMatrixView<Access, Key, N> operator()(const std::array<Key, N>& keys) const
    {
        return LocalMatrixView<Access, Key, N>(access, keys);
    }

    LocalMatrixEntry<Access, Key> operator()(Key key_row, Key key_col)
    {
        return LocalMatrixEntry<Access, Key>(access, key_row, key_col);
    }

    const LocalMatrixEntry<Access, Key> operator()(Key key_row, Key key_col) const
    {
        return LocalMatrixEntry<Access, Key>(access, key_row, key_col);
    }
};
