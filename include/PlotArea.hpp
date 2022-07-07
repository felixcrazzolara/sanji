/* 
 * Author: Felix Crazzolara
 */ 
#pragma once

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>

#include <vector>

#include "PlotData.hpp"
#include "LimitsInfo.hpp"

namespace sanji_ {

/* Type definitions */
template <typename T>
using vector = std::vector<T>;

class PlotArea : public QWidget {

Q_OBJECT

public:

/* Constructor */
explicit PlotArea(const vector<LineData>*  line_data,
                  const vector<ArrowData>* arrow_data,
                        LimitsInfo*        limits_info,
                        QWidget*           parent);

/* Setter */
void setBackgroundColor(const uint32_t color);
void setSizeHint(const QSize& size_hint);

/* Miscellaneous */
QSize sizeHint() const override;

protected:

void paintEvent(QPaintEvent* event)        override;
void mousePressEvent(QMouseEvent *event)   override;
void mouseReleaseEvent(QMouseEvent *event) override;
void mouseMoveEvent(QMouseEvent *event)    override;

private:

QColor background_color_;

const vector<LineData>*  line_data_;
const vector<ArrowData>* arrow_data_;
      LimitsInfo*        limits_info_;

QPoint selection_start_;
QPoint selection_end_;
bool   selection_active_;

QSize size_hint_;

};

};
