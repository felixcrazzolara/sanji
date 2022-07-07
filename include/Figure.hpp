#pragma once

#include <QWidget>
#include <QPen>

#include <Eigen/Dense>

#include <memory>
#include <vector>
#include <tuple>
#include <string>
#include <unordered_map>

#include "RenderArea.hpp"
#include "Colors.hpp"
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

class Figure : public QWidget {

Q_OBJECT

public:

/* Constructor and destructor */
Figure(const QString& fig_name="");
virtual ~Figure();

/* Plotting */
void plot(const VectorXd& x, const MatrixXd& y, const Style& style, const int priority);
void quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style, const int priority);

/* Setter */
void setAxesRatio(const std::string& axes_ratio);
void setxmin(const double xmin);
void setxmax(const double xmax);
void setymin(const double ymin);
void setymax(const double ymax);
void setPlotBackgroundColor(const uint32_t color);
void setxTicksBackgroundColor(const uint32_t color);
void setyTicksBackgroundColor(const uint32_t color);

protected:

void resizeEvent(QResizeEvent* event) override;

private:

friend RenderArea;

void checkLimitsAndPlotDataInfo();
void checkRenderArea();

QString                   fig_name_;
int                       current_render_area_idx_;
vector<RenderArea*>       render_areas_;
vector<LimitsInfo>        limits_info_;
vector<vector<LineData>>  line_data_;
vector<vector<ArrowData>> arrow_data_;

};

/* Type definitions */
using sh_fig_ptr = shared_ptr<Figure>;

};
