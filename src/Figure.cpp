#include <vector>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include "Figure.hpp"
#include "PlotArea.hpp"
#include "HTicksArea.hpp"
#include "VTicksArea.hpp"
#include "Colors.hpp"

namespace sanji_ {

using namespace sanji::colors;

/* Type definitions */
template <typename T>
using vector = std::vector<T>;

/* Global variables */
int                gbl_current_fig_index;
vector<sh_fig_ptr> gbl_fig_holder;
vector<bool>       gbl_free_fig_indices;

Figure::Figure(const QString& fig_name) {
    // Set miscellaneous variables
    current_render_area_idx_ = -1;

    // Make the figure visible
    this->show();

    // Set the global figure index
    for (uint i = 0; i < gbl_free_fig_indices.size(); ++i) {
        if (gbl_free_fig_indices[i]) {
            gbl_current_fig_index   = i;
            gbl_free_fig_indices[i] = false;
            fig_name_               = fig_name=="" ? QString::number(i) : fig_name;
            setWindowTitle(fig_name_);
            return;
        }
    }
    const uint previous_size = gbl_free_fig_indices.size();
    gbl_free_fig_indices.resize(2*previous_size,true);
    gbl_current_fig_index               = previous_size;
    gbl_free_fig_indices[previous_size] = false;
    fig_name_                           = fig_name=="" ? QString::number(previous_size) : fig_name;
    setWindowTitle(fig_name_);
}

Figure::~Figure() {
    for (const RenderArea* render_area : render_areas_)
        if (render_area) delete render_area;
}

void Figure::plot(const VectorXd& x, const MatrixXd& y, const Style& style, const int priority) {
    // Check the arguments
    if (x.rows() != y.rows()) throw new std::runtime_error("Figure::plot: x.rows() != y.rows()");

    // Configure the pen
    uint32_t color;
    if (style.find("color") == style.end()) color = BLACK;
    else                                    color = style.at("color");
    QPen pen(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff));
    if (style.find("line_style") == style.end()) pen.setStyle(Qt::SolidLine);
    else {
        if (style.at("line_style") == '-')      pen.setStyle(Qt::SolidLine);
        else if (style.at("line_style") == '.') pen.setStyle(Qt::DotLine);
        else throw new std::runtime_error("'"+std::to_string((char) style.at("line_style"))+"' is not a valid 'line_style'.");
    }

    // Make sure that a render area is available
    checkRenderArea();

    // Determine the min/max values
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    for (uint i = 0; i < x.rows(); ++i) {
        if (i == 0) {
            xmin = x(i);
            xmax = x(i);
        } else {
            if (x(i) < xmin) xmin = x(i);
            if (x(i) > xmax) xmax = x(i);
        }
        for (uint j = 0; j < y.cols(); ++j) {
            if (i == 0 && j == 0) {
                ymin = y(i,j);
                ymax = y(i,j);
            } else {
                if (y(i,j) < ymin) ymin = y(i,j);
                if (y(i,j) > ymax) ymax = y(i,j);
            }
        }
    }
    if (!limits_info_[current_render_area_idx_].value_init) {
        limits_info_[current_render_area_idx_].xmin_value = xmin;
        limits_info_[current_render_area_idx_].xmax_value = xmax;
        limits_info_[current_render_area_idx_].ymin_value = ymin;
        limits_info_[current_render_area_idx_].ymax_value = ymax;
        limits_info_[current_render_area_idx_].value_init = true;
    }
    if (!limits_info_[current_render_area_idx_].xmin_set && xmin < limits_info_[current_render_area_idx_].xmin)
        limits_info_[current_render_area_idx_].xmin = xmin - 0.025*(xmax-xmin);
    if (!limits_info_[current_render_area_idx_].xmax_set && xmax > limits_info_[current_render_area_idx_].xmax)
        limits_info_[current_render_area_idx_].xmax = xmax + 0.025*(xmax-xmin);
    if (!limits_info_[current_render_area_idx_].ymin_set && ymin < limits_info_[current_render_area_idx_].ymin)
        limits_info_[current_render_area_idx_].ymin = ymin - 0.025*(ymax-ymin);
    if (!limits_info_[current_render_area_idx_].ymax_set && ymax > limits_info_[current_render_area_idx_].ymax)
        limits_info_[current_render_area_idx_].ymax = ymax + 0.025*(ymax-ymin);
    if (limits_info_[current_render_area_idx_].xmin == limits_info_[current_render_area_idx_].xmax)
        if (limits_info_[current_render_area_idx_].xmin == 0) {
            limits_info_[current_render_area_idx_].xmin = -1.0-1e-3;
            limits_info_[current_render_area_idx_].xmax =  1.0+1e-3;
        } else {
            limits_info_[current_render_area_idx_].xmin = 0.9*limits_info_[current_render_area_idx_].xmin;
            limits_info_[current_render_area_idx_].xmax = 1.1*limits_info_[current_render_area_idx_].xmax;
        }
    if (limits_info_[current_render_area_idx_].ymin == limits_info_[current_render_area_idx_].ymax)
        if (limits_info_[current_render_area_idx_].ymin == 0) {
            limits_info_[current_render_area_idx_].ymin = -1.0-1e-3;
            limits_info_[current_render_area_idx_].ymax =  1.0+1e-3;
        } else {
            limits_info_[current_render_area_idx_].ymin = 0.9*limits_info_[current_render_area_idx_].ymin;
            limits_info_[current_render_area_idx_].ymax = 1.1*limits_info_[current_render_area_idx_].ymax;
        }

    // Copy the data
    const vec_ptr x_cpy = std::make_shared<VectorXd>();
    *x_cpy              = x;
    const mat_ptr y_cpy = std::make_shared<MatrixXd>();
    *y_cpy              = y;

    // Store the data
    line_data_[current_render_area_idx_].emplace_back(priority,x_cpy,y_cpy,pen);

    // Sort the data
    struct {
        bool operator()(const tuple<uint,vec_ptr,mat_ptr,QPen>& a, const tuple<uint,vec_ptr,mat_ptr,QPen>& b) const {
            return std::get<0>(a) < std::get<0>(b);
        }
    } custom_less;
    std::sort(line_data_[current_render_area_idx_].begin(),line_data_[current_render_area_idx_].end(),custom_less);
}

void Figure::quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style, const int priority) {
    // Check the arguments
    if (x.rows() != y.rows()) throw new std::runtime_error("Figure::quiver: x.rows() != y.rows()");
    if (x.rows() != u.rows()) throw new std::runtime_error("Figure::quiver: x.rows() != u.rows()");
    if (x.rows() != v.rows()) throw new std::runtime_error("Figure::quiver: x.rows() != v.rows()");

    // Parse the style arguments
    uint32_t color;
    if (style.find("color") == style.end()) color = BLACK;
    else                                    color = style.at("color");

    // Make sure that a render area is available
    checkRenderArea();

    // Determine the min/max values
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    for (uint i = 0; i < x.rows(); ++i) {
        if (i == 0) {
            xmin = x(i)+u(i);
            xmax = x(i)+u(i);
            ymin = y(i)+v(i);
            ymax = y(i)+v(i);
        } else {
            if (x(i)+u(i) < xmin) xmin = x(i)+u(i);
            if (x(i)+u(i) > xmax) xmax = x(i)+u(i);
            if (y(i)+v(i) < ymin) ymin = y(i)+v(i);
            if (y(i)+v(i) > ymax) ymax = y(i)+v(i);
        }
    }
    if (!limits_info_[current_render_area_idx_].value_init) {
        limits_info_[current_render_area_idx_].xmin_value = xmin;
        limits_info_[current_render_area_idx_].xmax_value = xmax;
        limits_info_[current_render_area_idx_].ymin_value = ymin;
        limits_info_[current_render_area_idx_].ymax_value = ymax;
        limits_info_[current_render_area_idx_].value_init = true;
    }

    // Possibly update the axis limits
    if (!limits_info_[current_render_area_idx_].xmin_set && xmin < limits_info_[current_render_area_idx_].xmin)
        limits_info_[current_render_area_idx_].xmin = xmin - 0.025*(xmax-xmin);
    if (!limits_info_[current_render_area_idx_].xmax_set && xmax > limits_info_[current_render_area_idx_].xmax)
        limits_info_[current_render_area_idx_].xmax = xmax + 0.025*(xmax-xmin);
    if (!limits_info_[current_render_area_idx_].ymin_set && ymin < limits_info_[current_render_area_idx_].ymin)
        limits_info_[current_render_area_idx_].ymin = ymin - 0.025*(ymax-ymin);
    if (!limits_info_[current_render_area_idx_].ymax_set && ymax > limits_info_[current_render_area_idx_].ymax)
        limits_info_[current_render_area_idx_].ymax = ymax + 0.025*(ymax-ymin);
    if (limits_info_[current_render_area_idx_].xmin == limits_info_[current_render_area_idx_].xmax)
        if (limits_info_[current_render_area_idx_].xmin == 0) {
            limits_info_[current_render_area_idx_].xmin = -1.0-1e-3;
            limits_info_[current_render_area_idx_].xmax =  1.0+1e-3;
        } else {
            limits_info_[current_render_area_idx_].xmin = 0.9*limits_info_[current_render_area_idx_].xmin;
            limits_info_[current_render_area_idx_].xmax = 1.1*limits_info_[current_render_area_idx_].xmax;
        }
    if (limits_info_[current_render_area_idx_].ymin == limits_info_[current_render_area_idx_].ymax)
        if (limits_info_[current_render_area_idx_].ymin == 0) {
            limits_info_[current_render_area_idx_].ymin = -1.0-1e-3;
            limits_info_[current_render_area_idx_].ymax =  1.0+1e-3;
        } else {
            limits_info_[current_render_area_idx_].ymin = 0.9*limits_info_[current_render_area_idx_].ymin;
            limits_info_[current_render_area_idx_].ymax = 1.1*limits_info_[current_render_area_idx_].ymax;
        }

    // Copy the data
    const vec_ptr x_cpy = std::make_shared<VectorXd>();
    *x_cpy              = x;
    const vec_ptr y_cpy = std::make_shared<VectorXd>();
    *y_cpy              = y;
    const vec_ptr u_cpy = std::make_shared<VectorXd>();
    *u_cpy              = u;
    const vec_ptr v_cpy = std::make_shared<VectorXd>();
    *v_cpy              = v;

    // Store the data
    arrow_data_[current_render_area_idx_].emplace_back(priority,x_cpy,y_cpy,u_cpy,v_cpy,QBrush(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff)));

    // Sort the data
    struct {
        bool operator()(const tuple<uint,vec_ptr,vec_ptr,vec_ptr,vec_ptr,QBrush>& a, const tuple<uint,vec_ptr,vec_ptr,vec_ptr,vec_ptr,QBrush>& b) const {
            return std::get<0>(a) < std::get<0>(b);
        }
    } custom_less;
    std::sort(arrow_data_[current_render_area_idx_].begin(),arrow_data_[current_render_area_idx_].end(),custom_less);
}

void Figure::resizeEvent(QResizeEvent* event) {
    if (current_render_area_idx_ >= 0) {
        const QRect& geom = geometry();
        render_areas_.back()->setGeometry(QRect(0,0,geom.width(),geom.height()));
    }
}

void Figure::checkRenderArea() {
    if (current_render_area_idx_ == -1) {
        line_data_.resize(1);
        arrow_data_.resize(1);
        limits_info_.emplace_back(LimitsInfo{std::numeric_limits<double>::max(),-std::numeric_limits<double>::max(),
                                             std::numeric_limits<double>::max(),-std::numeric_limits<double>::max(),
                                             0,0,0,0,false,false,false,false,false,LimitsInfo::AXES_RATIO::NONE});
        PlotArea* plot_area = new PlotArea(&line_data_[0], &arrow_data_[0], &limits_info_[0]);
        render_areas_.push_back(new RenderArea(this, plot_area, new HTicksArea(&limits_info_[0],plot_area), new VTicksArea(&limits_info_[0],plot_area)));
        const QRect& geom = geometry();
        render_areas_.back()->setGeometry(QRect(0,0,geom.width(),geom.height()));
        current_render_area_idx_ = 0;
    }
}

void Figure::setAxesRatio(const std::string& axes_ratio) {
    if (current_render_area_idx_ != -1)
        if (axes_ratio == "equal")
            limits_info_[current_render_area_idx_].axes_ratio = LimitsInfo::AXES_RATIO::EQUAL;
}

void Figure::setxmin(const double xmin) {
    if (current_render_area_idx_ != -1) {
        limits_info_[current_render_area_idx_].xmin     = xmin;
        limits_info_[current_render_area_idx_].xmin_set = true;
    }
}

void Figure::setxmax(const double xmax) {
    if (current_render_area_idx_ != -1) {
        limits_info_[current_render_area_idx_].xmax     = xmax;
        limits_info_[current_render_area_idx_].xmax_set = true;
    }
}

void Figure::setymin(const double ymin) {
    if (current_render_area_idx_ != -1) {
        limits_info_[current_render_area_idx_].ymin     = ymin;
        limits_info_[current_render_area_idx_].ymin_set = true;
    }
}

void Figure::setymax(const double ymax) {
    if (current_render_area_idx_ != -1) {
        limits_info_[current_render_area_idx_].ymax     = ymax;
        limits_info_[current_render_area_idx_].ymax_set = true;
    }
}

void Figure::setPlotBackgroundColor(const uint32_t color) {
    if (current_render_area_idx_ != -1)
        render_areas_[current_render_area_idx_]->setPlotBackgroundColor(color);
}

void Figure::setxTicksBackgroundColor(const uint32_t color) {
    if (current_render_area_idx_ != -1)
        render_areas_[current_render_area_idx_]->setxTicksBackgroundColor(color);
}

void Figure::setyTicksBackgroundColor(const uint32_t color) {
    if (current_render_area_idx_ != -1)
        render_areas_[current_render_area_idx_]->setyTicksBackgroundColor(color);
}

};
