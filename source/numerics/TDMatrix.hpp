#pragma once
#include <vector>
#include <cassert>
#include <stdexcept>

template<typename T>
class TDMatrix
{
public:
	TDMatrix(std::size_t n);
	std::size_t size() const;

	T get(std::size_t i, std::size_t j) const;
	void set(std::size_t i, std::size_t j, T val);

    void solve(std::vector<T>& rhs);

private:
	std::vector<T> a;	// Lower secondary diagonal
	std::vector<T> b;	// Diagonal
	std::vector<T> c;	// Upper secondary diagonal
};

template<typename T>
TDMatrix<T>::TDMatrix(std::size_t n): a(n-1), b(n), c(n-1)
{

}

template<typename T>
std::size_t TDMatrix<T>::size() const
{
	return b.size();
}

template<typename T>
T TDMatrix<T>::get(std::size_t i, std::size_t j) const
{
	assert(i < size() && j < size());
	
    switch(int(i-j))
	{
	case 1:
		return a[j];
	case 0:
		return b[j];
	case -1:
		return c[i];
	default:
		return 0;
	}
}

template<typename T>
void TDMatrix<T>::set(std::size_t i, std::size_t j, T val)
{
	assert(i < size() && j < size());

    switch(int(i-j))
	{
	case 1:
		a[j] = val;
		break;
	case 0:
		b[j] = val;
		break;
	case -1:
		c[i] = val;
		break;
	default:
        throw std::runtime_error("This entry cannot be set");   // Todo: Use something more assert-like here?
	}
}

template<typename T>
void TDMatrix<T>::solve(std::vector<T>& rhs)
{
	assert(rhs.size() == size());
	
	std::size_t n = size() - 1;

	c[0] /= b[0];
	rhs[0] /= b[0];

	for(std::size_t i = 1; i < n; ++i)
	{
		T m = b[i] - a[i-1]*c[i-1];
		c[i] = c[i]/m;
		rhs[i] = (rhs[i] - a[i-1]*rhs[i-1])/m;
	}

	rhs[n] = (rhs[n] - a[n-1]*rhs[n-1])/(b[n] - a[n-1]*c[n-1]);

	for(std::size_t i = n; i > 0; --i)
	{
		rhs[i-1] -= c[i-1]*rhs[i];
	}
}
