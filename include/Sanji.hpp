#pragma once

#include <QString>
#include <QApplication>
#include <Eigen/Dense>
#include "Figure.hpp"
#include "Colors.hpp"

namespace sanji {

using namespace sanji_;
using namespace sanji::colors;

/* Type definitions */
using VectorXd = Eigen::VectorXd;
using MatrixXd = Eigen::MatrixXd;

void init();

sh_fig_ptr figure();
sh_fig_ptr figure(const QString fig_name);

void plot(const VectorXd& x, const MatrixXd& y, const int priority=0, const char line_style='-', const Color color=BLACK);

};
