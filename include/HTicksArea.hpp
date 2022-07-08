#pragma once

#include "TicksArea.hpp"
#include "LimitsInfo.hpp"

namespace sanji_ {

class HTicksArea : public TicksArea {

Q_OBJECT

public:

/* Constructor */
explicit HTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent);

/* Getter */
uint getHeight() const;

uint getMinWidth() const;

/* Setter */
void setSizeHint(const QSize& size_hint);

/* Miscellaneous */
QSize sizeHint() const override;

protected:

void paintEvent(QPaintEvent* event) override;

private:

QSize size_hint_;

};

};
