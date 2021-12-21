#pragma once
#include <nlohmann/json.hpp>
#include <Eigen/Dense>

// TODO: Find out how to make generic implementations that are actually picked up by the json serializer...

namespace Eigen {

    using nlohmann::json;

    inline void to_json(json& object, const MatrixXd& matrix) {
        object = json::array();
        for(size_t i = 0; i < matrix.rows(); ++i) {
            object.push_back(json::array());
            for(size_t j = 0; j < matrix.cols(); ++i) {
                object[i][j] = matrix(i, j);
            }
        }
    }

    inline void from_json(const json& object, MatrixXd& matrix) {
        matrix = MatrixXd(object.size(), object.at(0).size());
        for(size_t i = 0; i < matrix.rows(); ++i) {
            for(size_t j = 0; j < matrix.cols(); ++j) {
                json value = object.at(i).at(j);
                matrix(i, j) = value.is_null() ? NAN : value.get<double>();
            }
        }
    }

    inline void to_json(json& object, const VectorXd& vector) {
        object = json::array();
        for(size_t i = 0; i < vector.size(); ++i) {
            object.push_back(vector[i]);
        }
    }

    inline void from_json(const json& object, VectorXd& vector) {
        vector = VectorXd(object.size());
        for(size_t i = 0; i < object.size(); ++i) {
            json value = object.at(i);
            vector[i] = value.is_null() ? NAN : value.get<double>();
        }
    }

    inline void to_json(json& object, const Vector2d& vector) {
        object = json::array();
        for(size_t i = 0; i < vector.size(); ++i) {
            object.push_back(vector[i]);
        }
    }

    inline void from_json(const json& object, Vector2d& vector) {
        for(size_t i = 0; i < object.size(); ++i) {
            json value = object.at(i);
            vector[i] = value.is_null() ? NAN : value.get<double>();
        }
    }
}
