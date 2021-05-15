#include "Figure.hpp"
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace sanji_ {

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

void Figure::plot(const VectorXd& x, const MatrixXd& y, const int priority, const char line_style, const Color color) {
    // Check the arguments
    if (x.rows() != y.rows()) throw new std::runtime_error("Figure::plot: x.rows() != y.rows()");

    // Make sure that a render area is available
    if (current_render_area_idx_ == -1) {
        data_.resize(1);
        limits_info_.emplace_back(LimitsInfo{0,0,0,0,false,false,false,false,false,false,false,false});
        render_areas_.push_back(new RenderArea(this,&data_[0],&limits_info_[0]));
        const QRect& geom = geometry();
        render_areas_.back()->setGeometry(QRect(geom.width()*0.05,geom.height()*0.05,geom.width()*0.9,geom.height()*0.9));
        current_render_area_idx_ = 0;
    }

    // Update the plot limits
    if (!limits_info_[current_render_area_idx_].xmin_init && !limits_info_[current_render_area_idx_].xmin_set) {
        limits_info_[current_render_area_idx_].xmin      = x(0);
        limits_info_[current_render_area_idx_].xmin_init = true;
    }
    if (!limits_info_[current_render_area_idx_].xmax_init && !limits_info_[current_render_area_idx_].xmax_set) {
        limits_info_[current_render_area_idx_].xmax      = x(0);
        limits_info_[current_render_area_idx_].xmax_init = true;
    }
    if (!limits_info_[current_render_area_idx_].ymin_init && !limits_info_[current_render_area_idx_].ymin_set) {
        limits_info_[current_render_area_idx_].ymin      = y(0,0);
        limits_info_[current_render_area_idx_].ymin_init = true;
    }
    if (!limits_info_[current_render_area_idx_].ymax_init && !limits_info_[current_render_area_idx_].ymax_set) {
        limits_info_[current_render_area_idx_].ymax      = y(0,0);
        limits_info_[current_render_area_idx_].ymax_init = true;
    }
    if (!limits_info_[current_render_area_idx_].xmin_set)
        for (uint i = 0; i < x.rows(); ++i)
            if (x(i) < limits_info_[current_render_area_idx_].xmin)
                limits_info_[current_render_area_idx_].xmin = x(i);
    if (!limits_info_[current_render_area_idx_].xmax_set)
        for (uint i = 0; i < x.rows(); ++i)
            if (x(i) > limits_info_[current_render_area_idx_].xmax)
                limits_info_[current_render_area_idx_].xmax = x(i);
    if (!limits_info_[current_render_area_idx_].ymin_set)
        for (uint i = 0; i < x.rows(); ++i)
            for (uint j = 0; j < y.cols(); ++j)
                if (y(i,j) < limits_info_[current_render_area_idx_].ymin)
                    limits_info_[current_render_area_idx_].ymin = y(i,j);
    if (!limits_info_[current_render_area_idx_].ymax_set)
        for (uint i = 0; i < x.rows(); ++i)
            for (uint j = 0; j < y.cols(); ++j)
                if (y(i,j) > limits_info_[current_render_area_idx_].ymax)
                    limits_info_[current_render_area_idx_].ymax = y(i,j);

    // Copy the data
    const vec_ptr x_cpy = std::make_shared<VectorXd>();
    *x_cpy              = x;
    const mat_ptr y_cpy = std::make_shared<MatrixXd>();
    *y_cpy              = y;

    // Store the data
    data_[current_render_area_idx_].emplace_back(priority,x_cpy,y_cpy,QPen(QColor(std::get<0>(color),std::get<1>(color),std::get<2>(color))));

    // Sort the data
    struct {
        bool operator()(const tuple<uint,vec_ptr,mat_ptr,QPen>& a, const tuple<uint,vec_ptr,mat_ptr,QPen>& b) const {
            return std::get<0>(a) < std::get<0>(b);
        }
    } custom_less;
    std::sort(data_[current_render_area_idx_].begin(),data_[current_render_area_idx_].end(),custom_less);
}

void Figure::resizeEvent(QResizeEvent* event) {
    if (current_render_area_idx_ >= 0) {
        const QRect& geom = geometry();
        render_areas_.back()->setGeometry(QRect(geom.width()*0.05,geom.height()*0.05,geom.width()*0.9,geom.height()*0.9));
    }
}

};
