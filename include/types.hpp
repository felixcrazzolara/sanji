/* 
 * Author: Felix Crazzolara
 */ 
#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include <Eigen/Dense>

namespace sanji_ {

/* Type definitions */
template <typename T>
using shared_ptr    = std::shared_ptr<T>;
using string        = std::string;
template <typename K, typename V>
using unordered_map = std::unordered_map<K,V>;

using VectorXd = Eigen::VectorXd;
using MatrixXd = Eigen::MatrixXd;
using vec_ptr  = shared_ptr<VectorXd>;
using mat_ptr  = shared_ptr<MatrixXd>;

};
