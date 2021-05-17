#include <QFontMetrics>
#include <QPainter>
#include "RenderArea.hpp"

namespace sanji_ {

RenderArea::RenderArea(QWidget*    parent,
                       PlotArea*   plot_area_,
                       HTicksArea* ticks_area_x,
                       VTicksArea* ticks_area_y) :
    QWidget(parent),
    plot_area_(plot_area_),
    tick_area_x_(ticks_area_x),
    tick_area_y_(ticks_area_y)
{
    plot_area_->setParent(this);
    tick_area_x_->setParent(this);
    tick_area_y_->setParent(this);
    this->show();
}

RenderArea::~RenderArea() {
    delete plot_area_;
    delete tick_area_x_;
    delete tick_area_y_;
}

void RenderArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // Fetch the geometry of the this widget
    const auto geom = geometry();

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255,255,255)));
    painter.drawRect(0,0,geom.width(),geom.height());
}

void RenderArea::resizeEvent(QResizeEvent* event) {
    const uint num_digits = 5;
    const uint tick_height = 11;
    const uint tick_width  = 11;
    char* buf = new char[num_digits];
    for (uint i = 0; i < num_digits-1; ++i) buf[i] = '0';
    buf[num_digits-1] = '\0';
    QFontMetrics fm(QFont("Monospace",10));
    const uint x_ticks_min_height = fm.boundingRect("0").height()+tick_height+1;
    const uint y_ticks_min_width  = fm.boundingRect(QString::fromStdString("-0."+std::string(buf)+"e-100")).width()+tick_width+1;
    delete[] buf;
    const auto geom               = geometry();
    const uint lw                 = y_ticks_min_width;
    const uint lh                 = x_ticks_min_height;
    plot_area_->setGeometry(lw,0,geom.width()-lw,geom.height()-lh);
    tick_area_y_->setGeometry(0,0,lw,geom.height()-lh);
    tick_area_x_->setGeometry(lw,geom.height()-lh,geom.width()-lw,lh);
}

};
