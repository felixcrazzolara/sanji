/* 
 * Author: Felix Crazzolara
 */ 
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

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
    if (x.rows() != y.rows()) {
        std::cout << "Figure::plot: x.rows() != y.rows()" << std::endl;
        abort();
    }

    // Make sure that a limits info and plot data objects are available
    checkLimitsAndPlotDataInfo();

    // Make sure that a render area is available

    // Update the limits
    limits_info_[current_render_area_idx_ == -1 ? 0 : current_render_area_idx_].update_limits(x, y);

    // Copy the data
    const vec_ptr x_cpy = std::make_shared<VectorXd>();
    *x_cpy              = x;
    const mat_ptr y_cpy = std::make_shared<MatrixXd>();
    *y_cpy              = y;

    // Store the data
    line_data_[current_render_area_idx_ == -1 ? 0 : current_render_area_idx_].emplace_back(
        priority,x_cpy,y_cpy,style);

    // Sort the data
    struct {
        bool operator()(const LineData& a, const LineData& b) const {
            return a.priority < b.priority;
        }
    } custom_less;
    {
        auto& line_data = line_data_[current_render_area_idx_== -1 ? 0 : current_render_area_idx_];
        std::sort(line_data.begin(),line_data.end(),custom_less);
    };

    // Make sure that a render area is available
    checkRenderArea();
}

void Figure::quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style, const int priority) {
    // Check the arguments
    if (x.rows() != y.rows()) throw new std::runtime_error("Figure::quiver: x.rows() != y.rows()");
    if (x.rows() != u.rows()) throw new std::runtime_error("Figure::quiver: x.rows() != u.rows()");
    if (x.rows() != v.rows()) throw new std::runtime_error("Figure::quiver: x.rows() != v.rows()");

    // Make sure that a limits info and plot data objects are available
    checkLimitsAndPlotDataInfo();

    // Check if a colormap shall be used
    bool use_colormap = false;
    double arrow_length = -1.0;
    if (style.find("use_colormap") != style.end()) {
        if (style.find("arrow_length") == style.end()) {
            throw new std::runtime_error("Must provided argument 'arrow_length' when using the option "
                                         "'use_colormap' for 'quiver'.");
        }
        use_colormap = true;
        arrow_length = style.at("arrow_length");
    } else {
        if (style.find("arrow_length") != style.end())
            arrow_length = style.at("arrow_length");
    }

    // Determine the min/max values
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    for (size_t i = 0; i < x.rows(); ++i) {
        double u_,v_;
        if (use_colormap || arrow_length > 0.0) {
            const double phi = std::atan2(v(i),u(i));
            u_               = std::cos(phi)*arrow_length;
            v_               = std::sin(phi)*arrow_length;
        } else {
            u_ = u(i);
            v_ = v(i);
        }

        if (i == 0) {
            xmin = x(i) + u_;
            xmax = x(i) + u_;
            ymin = y(i) + v_;
            ymax = y(i) + v_;
        } else {
            if (x(i) + u_ < xmin) xmin = x(i) + u_;
            if (x(i) + u_ > xmax) xmax = x(i) + u_;
            if (y(i) + v_ < ymin) ymin = y(i) + v_;
            if (y(i) + v_ > ymax) ymax = y(i) + v_;
        }
    }

    {
        // Get a reference to the current limits info object
        LimitsInfo& limits_info =
            limits_info_[current_render_area_idx_ == -1 ? 0 : current_render_area_idx_];

        // Possibly update the axis limits
        if (!limits_info.xmin_set && xmin < limits_info.xmin()) {
            limits_info.set_xmin(xmin - 0.025*(xmax-xmin));
        }
        if (!limits_info.xmax_set && xmax > limits_info.xmax()) {
            limits_info.set_xmax(xmax + 0.025*(xmax-xmin));
        }
        if (!limits_info.ymin_set && ymin < limits_info.ymin()) {
            limits_info.set_ymin(ymin - 0.025*(ymax-ymin));
        }
        if (!limits_info.ymax_set && ymax > limits_info.ymax()) {
            limits_info.set_ymax(ymax + 0.025*(ymax-ymin));
        }
    };

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
    arrow_data_[current_render_area_idx_ == -1 ? 0 : current_render_area_idx_].emplace_back(
        priority,x_cpy,y_cpy,u_cpy,v_cpy,style);

    // Sort the data
    struct {
        bool operator()(const ArrowData& a, const ArrowData& b) const {
            return a.priority < b.priority;
        }
    } custom_less;
    {
        auto& arrow_data = arrow_data_[current_render_area_idx_== -1 ? 0 : current_render_area_idx_];
        std::sort(arrow_data.begin(),arrow_data.end(),custom_less);
    };

    // Make sure that a render area is available
    checkRenderArea();
}

void Figure::resizeEvent(QResizeEvent* event) {
    if (current_render_area_idx_ >= 0) {
        const QRect& geom = geometry();
        render_areas_.back()->setGeometry(QRect(0,0,geom.width(),geom.height()));
    }
}

void Figure::checkLimitsAndPlotDataInfo() {
    if (current_render_area_idx_ == -1) {
        limits_info_.emplace_back(LimitsInfo());
        line_data_.resize(1);
        arrow_data_.resize(1);
    }
}

void Figure::checkRenderArea() {
    if (current_render_area_idx_ == -1) {
        render_areas_.push_back(new RenderArea(&line_data_[0], &arrow_data_[0], limits_info_[0], this));
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
        limits_info_[current_render_area_idx_].set_xmin(xmin);
        limits_info_[current_render_area_idx_].xmin_set = true;
    }
}

void Figure::setxmax(const double xmax) {
    if (current_render_area_idx_ != -1) {
        limits_info_[current_render_area_idx_].set_xmax(xmax);
        limits_info_[current_render_area_idx_].xmax_set = true;
    }
}

void Figure::setymin(const double ymin) {
    if (current_render_area_idx_ != -1) {
        limits_info_[current_render_area_idx_].set_ymin(ymin);
        limits_info_[current_render_area_idx_].ymin_set = true;
    }
}

void Figure::setymax(const double ymax) {
    if (current_render_area_idx_ != -1) {
        limits_info_[current_render_area_idx_].set_ymax(ymax);
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
