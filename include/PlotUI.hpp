#pragma once

#include <QWidget>
#include <QPoint>
#include <vector>
#include "LimitsInfo.hpp"

#define PLOT_UI_WIDTH  90
#define PLOT_UI_HEIGHT 20

namespace sanji_ {

/* Type definitions */
template <typename T>
using vector = std::vector<T>;

class PlotUI : public QWidget {

Q_OBJECT

public:

/* Constructor */
explicit PlotUI(LimitsInfo* limits_info, QWidget* parent);

protected:

void paintEvent(QPaintEvent* event)        override;
void mousePressEvent(QMouseEvent *event)   override;
void mouseReleaseEvent(QMouseEvent *event) override;

private:

int getButtonIndex(const QPoint& point) const;

QColor background_color_;

int press_index_;

LimitsInfo* limits_info_;

};

};
