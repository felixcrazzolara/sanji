#include <QFontMetrics>
#include <QPainter>
#include "RenderArea.hpp"

namespace sanji_ {

RenderArea::RenderArea(const LineData*   line_data,
                       const ArrowData*  arrow_data,
                             LimitsInfo& limits_info,
                             QWidget*    parent) :
    QWidget(parent),
    plot_area_(new PlotArea(line_data, arrow_data, &limits_info, this)),
    tick_area_x_(new HTicksArea(&limits_info, plot_area_, this)),
    tick_area_y_(new VTicksArea(&limits_info, plot_area_, this)),
    plot_ui_(new PlotUI(&limits_info, this))
{
    this->show();
}

RenderArea::~RenderArea() {}

void RenderArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

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
    plot_ui_->setGeometry(geom.width()-PLOT_UI_WIDTH,0,PLOT_UI_WIDTH,PLOT_UI_HEIGHT);
}

void RenderArea::setPlotBackgroundColor(const uint32_t color) {
    plot_area_->setBackgroundColor(color);
}

void RenderArea::setxTicksBackgroundColor(const uint32_t color) {
    tick_area_x_->setBackgroundColor(color);
}

void RenderArea::setyTicksBackgroundColor(const uint32_t color) {
    tick_area_y_->setBackgroundColor(color);
}

};
