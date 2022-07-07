/* 
 * Author: Felix Crazzolara
 */ 
#pragma once

#include <limits>
#include <cmath>

#include <types.hpp>

namespace sanji_ {

/* Type definitions */

using ScalingsAndLimits = tuple<double,double,double,double,double,double>;

struct AxesLimits {

AxesLimits() : xmin{ std::numeric_limits<double>::max()},
               xmax{-std::numeric_limits<double>::max()},
               ymin{ std::numeric_limits<double>::max()},
               ymax{-std::numeric_limits<double>::max()}
{}

AxesLimits(const double xmin, const double xmax, const double ymin, const double ymax) :
    xmin{xmin},xmax{xmax},ymin{ymin},ymax{ymax}
{}

double xmin;
double xmax;
double ymin;
double ymax;

};

class LimitsInfo {

public:

enum class AXES_RATIO {NONE,EQUAL};

LimitsInfo() : xmin_value{ std::numeric_limits<double>::max()},
               xmax_value{-std::numeric_limits<double>::max()},
               ymin_value{ std::numeric_limits<double>::max()},
               ymax_value{-std::numeric_limits<double>::max()},
               xmin_set{false},
               xmax_set{false},
               ymin_set{false},
               ymax_set{false},
               axes_ratio{AXES_RATIO::NONE},
               hist_idx{0}
{
    axes_limits_hist.resize(1);
}

ScalingsAndLimits getScalingsAndLimits(const double plot_width_px, const double plot_height_px) const {
    // Define some variables for convenience
    const bool   x_both_set = xmin_set && xmax_set;
    const bool   y_both_set = ymin_set && ymax_set;
          double xplot_min  = xmin();
          double xplot_max  = xmax();
          double yplot_min  = ymin();
          double yplot_max  = ymax();
          double xplot_mean = (xplot_max+xplot_min)/2.0;
          double yplot_mean = (yplot_max+yplot_min)/2.0;
          double dx_to_px   = plot_width_px / (xmax() - xmin());
          double dy_to_px   = plot_height_px / (ymax() - ymin());
    if (x_both_set && y_both_set) {
        // Determine the correct scalings
        // TODO: What if one of the denominators is zero?
        if (axes_ratio == AXES_RATIO::EQUAL) {
            if (dx_to_px > dy_to_px) {
                dx_to_px  = dy_to_px;
                xplot_min = xplot_mean - plot_width_px/2.0/dx_to_px;
                xplot_max = xplot_mean + plot_width_px/2.0/dx_to_px;
            } else if (dx_to_px < dy_to_px) {
                dy_to_px  = dx_to_px;
                yplot_min = yplot_mean - plot_height_px/2.0/dy_to_px;
                yplot_max = yplot_mean + plot_height_px/2.0/dy_to_px;
            }
        }
        if (axes_ratio == AXES_RATIO::EQUAL) dx_to_px = dy_to_px = std::min(dx_to_px,dy_to_px);
    } else if (x_both_set) {
        if (axes_ratio == AXES_RATIO::EQUAL) {
            dy_to_px = dx_to_px;
            if (ymin_set)      yplot_max = yplot_min + plot_height_px/dy_to_px;
            else if (ymax_set) yplot_min = yplot_max - plot_height_px/dy_to_px;
            else {
                               yplot_min = (yplot_max+yplot_min)/2.0 - plot_height_px/2.0/dy_to_px;
                               yplot_max = (yplot_max+yplot_min)/2.0 + plot_height_px/2.0/dy_to_px;
            }
        }
    } else if (y_both_set) {
        if (axes_ratio == AXES_RATIO::EQUAL) {
            dx_to_px = dy_to_px;
            if (xmin_set)      xplot_max = xplot_min + plot_width_px/dx_to_px;
            else if (xmax_set) xplot_min = xplot_max - plot_width_px/dx_to_px;
            else {
                               xplot_min = (xplot_max+xplot_min)/2.0 - plot_width_px/2.0/dx_to_px;
                               xplot_max = (xplot_max+xplot_min)/2.0 + plot_width_px/2.0/dx_to_px;
            }
        }
    } else {
        if (axes_ratio == AXES_RATIO::EQUAL) {
            dx_to_px = dy_to_px = std::min(dx_to_px,dy_to_px);
            if (xmin_set)      xplot_max = xplot_min + plot_width_px/dx_to_px;
            else if (xmax_set) xplot_min = xplot_max - plot_width_px/dx_to_px;
            else {
                               xplot_min = (xplot_max+xplot_min)/2.0 - plot_width_px/2.0/dx_to_px;
                               xplot_max = (xplot_max+xplot_min)/2.0 + plot_width_px/2.0/dx_to_px;
            }
            if (ymin_set)      yplot_max = yplot_min + plot_height_px/dy_to_px;
            else if (ymax_set) yplot_min = yplot_max - plot_height_px/dy_to_px;
            else {
                               yplot_min = (yplot_max+yplot_min)/2.0 - plot_height_px/2.0/dy_to_px;
                               yplot_max = (yplot_max+yplot_min)/2.0 + plot_height_px/2.0/dy_to_px;
            }
        }
    }
    return {xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px};
}

void update_limits(const VectorXd& x, const MatrixXd& y) {
    // Determine the min/max values
    const double xmin = x.minCoeff();
    const double xmax = x.maxCoeff();
    const double ymin = y.minCoeff();
    const double ymax = y.maxCoeff();

    // Update axis limits which are not fixed
    if (!xmin_set && xmin < this->xmin()) {
        set_xmin(xmin - 0.025*(xmax-xmin));
    }
    if (!xmax_set && xmax > this->xmax()) {
        set_xmax(xmax + 0.025*(xmax-xmin));
    }
    if (!ymin_set && ymin < this->ymin()) {
        set_ymin(ymin - 0.025*(ymax-ymin));
    }
    if (!ymax_set && ymax > this->ymax()) {
        set_ymax(ymax + 0.025*(ymax-ymin));
    }
}

double xmin() const { return axes_limits_hist[hist_idx].xmin; }
double xmax() const { return axes_limits_hist[hist_idx].xmax; }
double ymin() const { return axes_limits_hist[hist_idx].ymin; }
double ymax() const { return axes_limits_hist[hist_idx].ymax; }

void set_xmin(const double xmin) { axes_limits_hist[hist_idx].xmin = xmin; }
void set_xmax(const double xmax) { axes_limits_hist[hist_idx].xmax = xmax; }
void set_ymin(const double ymin) { axes_limits_hist[hist_idx].ymin = ymin; }
void set_ymax(const double ymax) { axes_limits_hist[hist_idx].ymax = ymax; }

void add_axes_limits(const double xmin, const double xmax, const double ymin, const double ymax) {
    if (hist_idx < axes_limits_hist.size()-1) {
        hist_idx += 1;
        axes_limits_hist[hist_idx].xmin = xmin;
        axes_limits_hist[hist_idx].xmax = xmax;
        axes_limits_hist[hist_idx].ymin = ymin;
        axes_limits_hist[hist_idx].ymax = ymax;
    } else {
        axes_limits_hist.push_back(AxesLimits{xmin,xmax,ymin,ymax});
        hist_idx += 1;
    }
}

void decrease_hist_index_if_possible() {
    if (hist_idx > 0) --hist_idx;
}

void increase_hist_index_if_possible() {
    if (hist_idx < axes_limits_hist.size()-1) ++hist_idx;
}

void reset_hist_index() {
    hist_idx = 0;
}

double     xmin_value;
double     xmax_value;
double     ymin_value;
double     ymax_value;

bool       xmin_set;
bool       xmax_set;
bool       ymin_set;
bool       ymax_set;

AXES_RATIO axes_ratio;

private:

vector<AxesLimits> axes_limits_hist;
uint               hist_idx;

};

};
