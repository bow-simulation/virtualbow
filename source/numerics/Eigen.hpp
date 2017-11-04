#pragma once
#define EIGEN_MAX_ALIGN_BYTES 0    // Turn off alignment
#include <Eigen/Dense>

#include<json.hpp>

using nlohmann::json;

template<size_t n, size_t k = n>
using Matrix = Eigen::Matrix<double, n, k>;

template<std::size_t n>
using Vector = Eigen::Matrix<double, n, 1>;

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::VectorXi;    // Todo: Remove?
using Eigen::Ref;

namespace Eigen
{
    static void to_json(json& obj, const VectorXd& vec)
    {
        obj = json::array();
        for(size_t i = 0; i < vec.size(); ++i)
            obj.push_back(vec[i]);
    }

    static void from_json(const json& obj, VectorXd& vec)
    {
        vec = VectorXd(obj.size());
        for(size_t i = 0; i < vec.size(); ++i)
            vec[i] = obj[i];
    }
}

static VectorXd unit_vector(size_t n, size_t i)
{
    assert(i < n);

    VectorXd vec = VectorXd::Zero(n);
    vec(i) = 1.0;

    return vec;
}
