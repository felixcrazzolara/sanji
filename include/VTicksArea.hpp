#pragma once

#include "TicksArea.hpp"
#include "LimitsInfo.hpp"

namespace sanji_ {

class VTicksArea : public TicksArea {

Q_OBJECT

public:

/* Constructor */
explicit VTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent);

/* Getter */
uint getXMargin() const;
uint getYMargin() const;
uint getMinHeight() const;

/* Setter */
void setSizeHint(const QSize& size_hint);

/* Miscellaneous */
QSize sizeHint() const override;

protected:

void paintEvent(QPaintEvent* event) override;

void resizeEvent(QResizeEvent* event) override;

private:

friend class RenderArea;

uint getMaxLabelWidth(const uint plot_area_width, const uint plot_area_height);

void getLabel(char* chr_buffer, const double y) const;

int   last_width_;
QSize size_hint_;

};

};
