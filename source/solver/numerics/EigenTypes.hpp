#pragma once
#include <Eigen/Dense>

template<size_t n, size_t k>
using Matrix = Eigen::Matrix<double, n, k>;

template<size_t n, size_t k>
using Array = Eigen::Array<double, n, k>;

template<std::size_t n>
using Vector = Eigen::Matrix<double, n, 1>;

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;
using Eigen::Ref;
