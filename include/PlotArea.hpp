/* 
 * Author: Felix Crazzolara
 */ 
#pragma once

#include "LimitsInfo.hpp"
#include "PlotData.hpp"

#include "Image.hpp"

#include <QImage>
#include <QMouseEvent>
#include <QPoint>
#include <QWidget>

namespace sanji_ {

class PlotArea : public QWidget {

Q_OBJECT

public:

/* Constructors */
explicit PlotArea(QWidget *parent, LimitsInfo *limits_info, const Image &image);

explicit PlotArea(QWidget *parent, const LineDataWrapper *line_data, const ArrowDataWrapper *arrow_data,
    LimitsInfo *limits_info);

/* Setter */
void setBackgroundColor(const uint32_t color);
void setSizeHint(const QSize &size_hint);

/* Miscellaneous */
QSize sizeHint() const override;

protected:

void paintEvent(QPaintEvent *event)        override;
void mousePressEvent(QMouseEvent *event)   override;
void mouseReleaseEvent(QMouseEvent *event) override;
void mouseMoveEvent(QMouseEvent *event)    override;

private:

void plotLineData(QPainter &painter, const QRect &geom, const ScalingsAndLimits &s);

void plotArrowData(QPainter &painter, const QRect &geom, const ScalingsAndLimits &s);

void plotImageData(QPainter &painter, const QRect &geom);

QColor background_color_;

const LineDataWrapper *line_data_;
const ArrowDataWrapper *arrow_data_;
const QImage image_;
LimitsInfo *limits_info_;

QPoint selection_start_;
QPoint selection_end_;
bool   selection_active_;

QSize size_hint_;

};

};
