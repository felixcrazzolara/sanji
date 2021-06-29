#pragma once

#include <QWidget>
#include "PlotArea.hpp"
#include "HTicksArea.hpp"
#include "VTicksArea.hpp"
#include "PlotUI.hpp"

namespace sanji_ {

class RenderArea : public QWidget {

Q_OBJECT

public:

/* Constructor and destructor */
explicit RenderArea(QWidget*    parent,
                    PlotArea*   plot_area_,
                    HTicksArea* ticks_area_x,
                    VTicksArea* ticks_area_y);
~RenderArea();                    

/* Setter */
void setPlotBackgroundColor(const uint32_t color);
void setxTicksBackgroundColor(const uint32_t color);
void setyTicksBackgroundColor(const uint32_t color);

protected:

void paintEvent(QPaintEvent* event) override;

void resizeEvent(QResizeEvent* event) override;

private:

PlotArea*   plot_area_;
HTicksArea* tick_area_x_;
VTicksArea* tick_area_y_;
PlotUI*     plot_ui_;

};

};
