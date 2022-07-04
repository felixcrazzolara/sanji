#include "TicksArea.hpp"

namespace sanji_ {

TicksArea::TicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent) :
    QWidget(parent),
    background_color_{255,255,255},
    limits_info_(limits_info),
    plot_area_(plot_area)
{}

void TicksArea::setBackgroundColor(const uint32_t color) {
    background_color_ = QColor(0xffu&(color>>16),0xffu&(color>>8),0xffu&color);
}

};
