/* 
 * Author: Felix Crazzolara
 */ 
#pragma once

#include <QFrame>
#include <QPoint>

#include "LimitsInfo.hpp"

namespace sanji_ {

class PlotUI : public QFrame {

Q_OBJECT

public:

/* Constructor */
explicit PlotUI(LimitsInfo* limits_info, QWidget* parent);

protected:

void paintEvent(QPaintEvent* event)        override;
void mousePressEvent(QMouseEvent *event)   override;
void mouseReleaseEvent(QMouseEvent *event) override;

private:

enum class ButtonIndex {INVALID, HOME, BACKWARD, FORWARD};

ButtonIndex getButtonIndex(const QPoint& point) const;

QColor background_color_;

ButtonIndex press_idx_;

LimitsInfo* limits_info_;

uint limit_x1_;
uint limit_x2_;

};

};
