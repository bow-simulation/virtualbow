#pragma once
#include <Eigen/Core>

#include <array>
#include <functional>

// Todo: Not actually needed here
using Eigen::VectorXd;
using Eigen::MatrixXd;

template<std::size_t n>
using Matrix = Eigen::Matrix<double, n, n>;

template<std::size_t n>
using Vector = Eigen::Matrix<double, n, 1>;

//================================================================================================

template<class Key>
class LocalVectorEntry
{
public:
    std::function<double(Key)>& get;
    std::function<void(Key, double)>& set;
    std::function<void(Key, double)>& add;
    Key key;

    // Read
    operator double() const
    {
        return get(key);
    }

    // Write
    LocalVectorEntry<Key>& operator=(double rhs)
    {
        set(key, rhs);
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

template<class Key, size_t N>
class LocalVectorView
{
public:
    std::function<double(Key)>& get;
    std::function<void(Key, double)>& set;
    std::function<void(Key, double)>& add;
    const std::array<Key, N>& keys;

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
    LocalVectorEntry<Key> operator()(size_t i)
    {
        return LocalVectorEntry<Key>{get, set, add, keys[i]};
    }

    const LocalVectorEntry<Key> operator()(size_t i) const
    {
        return LocalVectorEntry<Key>{get, set, add, keys[i]};
    }
};

template<typename Key>
class VectorView
{
private:
    mutable std::function<double(Key)> get;
    mutable std::function<void(Key, double)> set;
    mutable std::function<void(Key, double)> add;

public:
    VectorView(std::function<double(Key)> get,
               std::function<void(Key, double)> set,
               std::function<void(Key, double)> add)
        : get(get),
          set(set),
          add(add)
    {

    }

    // Multi index
    template<size_t N>
    LocalVectorView<Key, N> operator()(const std::array<Key, N>& keys)
    {
        return LocalVectorView<Key, N>{get, set, add, keys};
    }

    template<size_t N>
    const LocalVectorView<Key, N> operator()(const std::array<Key, N>& keys) const
    {
        return LocalVectorView<Key, N>{get, set, add, keys};
    }

    // Single indices
    LocalVectorEntry<Key> operator()(Key key)
    {
        return LocalVectorEntry<Key>{get, set, add, key};
    }

    const LocalVectorEntry<Key> operator()(Key key) const
    {
        return LocalVectorEntry<Key>{get, set, add, key};
    }
};

//================================================================================================

template<class Key>
class LocalMatrixEntry
{
public:
    std::function<void(Key, Key, double)>& add;
    Key key_row;
    Key key_col;

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


template<class Key, size_t N>
class LocalMatrixView
{
public:
    std::function<void(Key, Key, double)>& add;
    const std::array<Key, N>& keys;

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
    LocalMatrixEntry<Key> operator()(size_t i, size_t j)
    {
        return LocalMatrixEntry<Key>{add, keys[i], keys[j]};
    }

    const LocalMatrixEntry<Key> operator()(size_t i, size_t j) const
    {
        return LocalMatrixEntry<Key>{add, keys[i], keys[j]};
    }
};

template<class Key>
class MatrixView
{
private:
    mutable std::function<void(Key, Key, double)> add;

public:
    MatrixView(std::function<void(Key, Key, double)> add)
        : add(add)
    {

    }

    // Index
    template<size_t N>
    LocalMatrixView<Key, N> operator()(const std::array<Key, N>& keys)
    {
        return LocalMatrixView<Key, N>{add, keys};
    }

    template<size_t N>
    const LocalMatrixView<Key, N> operator()(const std::array<Key, N>& keys) const
    {
        return LocalMatrixView<Key, N>{add, keys};
    }

    LocalMatrixEntry<Key> operator()(Key key_row, Key key_col)
    {
        return LocalMatrixEntry<Key>{add, key_row, key_col};
    }

    const LocalMatrixEntry<Key> operator()(Key key_row, Key key_col) const
    {
        return LocalMatrixEntry<Key>{add, key_row, key_col};
    }
};
