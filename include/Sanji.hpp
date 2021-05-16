#pragma once

#include <QString>
#include <QApplication>
#include <Eigen/Dense>
#include <string>
#include <unordered_map>
#include "Figure.hpp"

namespace sanji {

using namespace sanji_;

/* Type definitions */
using VectorXd = Eigen::VectorXd;
using MatrixXd = Eigen::MatrixXd;
using Style    = std::unordered_map<std::string,double>;

void init();

sh_fig_ptr figure();
sh_fig_ptr figure(const QString fig_name);

void plot(const VectorXd& x, const MatrixXd& y, const Style& style, const int priority=0);

void quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style, const int priority=0);

void printAvailableFontFamilies();

void setAxisRatio(const std::string& axis_ratio);

};
