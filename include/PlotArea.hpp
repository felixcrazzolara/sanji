#pragma once

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <Eigen/Dense>
#include <tuple>
#include <vector>
#include <memory>
#include <unordered_set>
#include <string>
#include "LimitsInfo.hpp"

namespace sanji_ {

/* Type definitions */
template <typename T>
using shared_ptr = std::shared_ptr<T>;
template <typename T>
using vector     = std::vector<T>;
using VectorXd   = Eigen::VectorXd;
using MatrixXd   = Eigen::MatrixXd;
using vec_ptr    = shared_ptr<VectorXd>;
using mat_ptr    = shared_ptr<MatrixXd>;
template <class... Types>
using tuple      = std::tuple<Types...>;
using Style      = std::unordered_map<std::string,double>;

class PlotArea : public QWidget {

Q_OBJECT

public:

/* Constructor */
explicit PlotArea(const vector<tuple<uint,vec_ptr,mat_ptr,Style>>*                 line_data,
                  const vector<tuple<uint,vec_ptr,vec_ptr,vec_ptr,vec_ptr,Style>>* arrow_data,
                  LimitsInfo*                                                      limits_info);

/* Setter */
void setBackgroundColor(const uint32_t color);

protected:

void paintEvent(QPaintEvent* event)        override;
void mousePressEvent(QMouseEvent *event)   override;
void mouseReleaseEvent(QMouseEvent *event) override;
void mouseMoveEvent(QMouseEvent *event)    override;

private:

QColor                                                           background_color_;

const vector<tuple<uint,vec_ptr,mat_ptr,Style>>*                 line_data_;
const vector<tuple<uint,vec_ptr,vec_ptr,vec_ptr,vec_ptr,Style>>* arrow_data_;
LimitsInfo*                                                      limits_info_;

QPoint                                                           selection_start_;
QPoint                                                           selection_end_;
bool                                                             selection_active_;

};

};
