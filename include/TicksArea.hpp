#pragma once

#include <QWidget>
#include <QLabel>
#include <vector>
#include "PlotArea.hpp"

namespace sanji_ {

// Forward declarations
struct LimitsInfo;

/* Type definitions */
template <typename T>
using vector = std::vector<T>;

class TicksArea : public QWidget {

Q_OBJECT

public:

/* Constructor */
explicit TicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area);

/* Setter */
void setBackgroundColor(const uint32_t color);

protected:

QColor            background_color_;

vector<QLabel*>   tick_labels_;
const LimitsInfo* limits_info_;
const PlotArea*   plot_area_;

};

};
