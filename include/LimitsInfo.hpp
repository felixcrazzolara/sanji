#pragma once

#include <vector>
#include <limits>

namespace sanji_ {

struct AxesLimits {

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
               value_init{false},
               xmin_set{false},
               xmax_set{false},
               ymin_set{false},
               ymax_set{false},
               axes_ratio{AXES_RATIO::NONE},
               hist_idx{0}
{
    axes_limits_hist.resize(1);
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

bool       value_init;

bool       xmin_set;
bool       xmax_set;
bool       ymin_set;
bool       ymax_set;

AXES_RATIO axes_ratio;

private:

std::vector<AxesLimits> axes_limits_hist;
uint                    hist_idx;

};

};
