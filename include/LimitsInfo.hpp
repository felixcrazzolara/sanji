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

void set_xmin(double xmin) { axes_limits_hist[hist_idx].xmin = xmin; }
void set_xmax(double xmax) { axes_limits_hist[hist_idx].xmax = xmax; }
void set_ymin(double ymin) { axes_limits_hist[hist_idx].ymin = ymin; }
void set_ymax(double ymax) { axes_limits_hist[hist_idx].ymax = ymax; }

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
