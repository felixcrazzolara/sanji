#pragma once

#include <QWidget>
#include <QLabel>
#include <vector>
#include "PlotArea.hpp"

namespace sanji_ {

/* Type definitions */
template <typename T>
using vector     = std::vector<T>;

// Forward declarations
struct LimitsInfo;

class HTicksArea : public QWidget {

Q_OBJECT

public:

/* Constructor */
explicit HTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area);

protected:

void paintEvent(QPaintEvent* event) override;

private:

const LimitsInfo* limits_info_;
vector<QLabel*>   tick_labels_;
const PlotArea*   plot_area_;

};

};
