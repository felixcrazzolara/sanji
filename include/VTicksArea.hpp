#pragma once

#include "TicksArea.hpp"
#include "LimitsInfo.hpp"

namespace sanji_ {

class VTicksArea : public TicksArea {

Q_OBJECT

public:

/* Constructor */
explicit VTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent);

protected:

void paintEvent(QPaintEvent* event) override;

};

};
