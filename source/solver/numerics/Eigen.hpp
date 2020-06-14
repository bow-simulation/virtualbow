#pragma once
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

using nlohmann::json;

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

namespace Eigen
{
    // Allow comparing matrices of different dimension, return false on mismatch
    static bool operator==(const MatrixXd& lhs, const MatrixXd& rhs)
    {
        if((rhs.rows() != lhs.rows()) || (lhs.cols() != rhs.cols()))
            return false;

        return lhs.operator==(rhs);
    }

    static bool operator!=(const MatrixXd& lhs, const MatrixXd& rhs)
    {
        return !operator==(lhs, rhs);
    }

    static void to_json(json& obj, const VectorXd& vec)
    {
        obj = json::array();
        for(size_t i = 0; i < vec.size(); ++i)
        {
            obj.push_back(vec[i]);
        }
    }

    static void from_json(const json& obj, VectorXd& vec)
    {
        vec = VectorXd(obj.size());
        for(size_t i = 0; i < obj.size(); ++i)
        {
            vec[i] = obj.at(i);
        }
    }

    // Using row major order, like numpy
    static void to_json(json& obj, const MatrixXd& mat)
    {
        obj = json::array();
        for(size_t i = 0; i < mat.rows(); ++i)
        {
            obj.push_back(json::array());
            for(size_t j = 0; j < mat.cols(); ++j)
            {
                obj[i][j] = mat(i, j);
            }
        }
    }

    // Using row major order, like numpy
    static void from_json(const json& obj, MatrixXd& mat)
    {
        mat = MatrixXd(obj.size(), obj.at(0).size());
        for(size_t i = 0; i < mat.rows(); ++i)
        {
            for(size_t j = 0; j < mat.cols(); ++j)
            {
                mat(i, j) = obj.at(i).at(j);
            }
        }
    }
}

static VectorXd unit_vector(size_t n, size_t i)
{
    assert(i < n);

    VectorXd vec = VectorXd::Zero(n);
    vec(i) = 1.0;

    return vec;
}
