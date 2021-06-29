#pragma once

#include "TicksArea.hpp"
#include "LimitsInfo.hpp"

namespace sanji_ {

class HTicksArea : public TicksArea {

Q_OBJECT

public:

/* Constructor */
explicit HTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area);

protected:

void paintEvent(QPaintEvent* event) override;

};

};
