#pragma once

#include "TicksArea.hpp"

namespace sanji_ {

class VTicksArea : public TicksArea {

Q_OBJECT

public:

/* Constructor */
explicit VTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area);

protected:

void paintEvent(QPaintEvent* event) override;

};

};
