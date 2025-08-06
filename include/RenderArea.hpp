#pragma once

#include "HTicksArea.hpp"
#include "PlotArea.hpp"
#include "PlotUI.hpp"
#include "VTicksArea.hpp"

#include "Image.hpp"

#include <QWidget>

namespace sanji_ {

class RenderArea : public QWidget {

Q_OBJECT

public:

RenderArea(const Image &image, LimitsInfo &limits_info, QWidget *parent = nullptr);

explicit RenderArea(const LineDataWrapper *line_data, const ArrowDataWrapper *arrow_data,
    LimitsInfo &limits_info, QWidget *parent = nullptr);

/* Setter */
void setPlotBackgroundColor(const uint32_t color);
void setxTicksBackgroundColor(const uint32_t color);
void setyTicksBackgroundColor(const uint32_t color);

/* Miscellaneous */
void updateContent();

protected:

void paintEvent(QPaintEvent *event) override;

void resizeEvent(QResizeEvent *event) override;

private:

const LineDataWrapper *line_data_;
const ArrowDataWrapper *arrow_data_;

PlotArea *plot_area_;
HTicksArea *tick_area_x_;
VTicksArea *tick_area_y_;
PlotUI *plot_ui_;

};

};
