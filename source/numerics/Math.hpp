#pragma once
#include <Eigen/Dense>
#include <json.hpp>

using nlohmann::json;

template<size_t n, size_t k = n>
using Matrix = Eigen::Matrix<double, n, k>;

template<std::size_t n>
using Vector = Eigen::Matrix<double, n, 1>;

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::VectorXi;

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
