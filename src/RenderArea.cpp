#include <QFontMetrics>
#include <QPainter>

#include "RenderArea.hpp"

namespace sanji_ {

RenderArea::RenderArea(const vector<LineData>*  line_data,
                       const vector<ArrowData>* arrow_data,
                             LimitsInfo&        limits_info,
                             QWidget*           parent) :
    QWidget(parent),
    plot_area_(new PlotArea(line_data, arrow_data, &limits_info, this)),
    tick_area_x_(new HTicksArea(&limits_info, plot_area_, this)),
    tick_area_y_(new VTicksArea(&limits_info, plot_area_, this)),
    plot_ui_(new PlotUI(&limits_info, this))
{
    this->show();
}

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
    // TODO: Put these settings somewhere else
    const uint num_digits  = 5;
    const uint tick_height = 11;
    const uint tick_width  = 11;

    // Compute the height of the horizontal tick labels
    QFontMetrics fm(QFont("Monospace",10));
    const uint x_ticks_area_height = fm.boundingRect("0").height() + tick_height + 1;

    // Get the geometry of the render area
    const auto geom = geometry();

    // Here is a tricky situation. The tick labels depend on the size of the plot area, which in turn is
    // determined by the width of the vertical tick area. Optimally, the width of the vertical tick area
    // should be based on the length of the longest tick label. However, this causes a circular dependency.
    //
    // The solution used is as follows: Under normal circumstances, the width of the vertical tick area
    // as drawn before is used as an initial guess. If this width is not available, a default width is
    // used as the initial guess. Based on this guess, the width of the plot area can be computed. The
    // height of the plot area is already known. Based on this estimated size of the plot area, the tick
    // labels are computed. The width of the longest tick label is then used to determine the actual new
    // width of the vertical tick area.

    // Estimate the plot area
    const uint plot_area_height = geom.height() - x_ticks_area_height;
    uint plot_area_width_guess;
    if (tick_area_y_->last_width_ == -1) {
        char* buf = new char[num_digits];
        for (size_t i = 0; i < num_digits-1; ++i) buf[i] = '0';
        buf[num_digits-1] = '\0';
        plot_area_width_guess = geom.width() - fm.boundingRect(QString::fromStdString(
            "-0."+std::string(buf)+"e-100")).width() + tick_width + 2*tick_area_y_->getXMargin();
        delete[] buf;
    } else {
        plot_area_width_guess = geom.width() - tick_area_y_->last_width_;
    }

    // Compute the maximum y-tick label width based on the estimate of the size of the plot area
    const uint max_ytick_label_width = tick_area_y_->getMaxLabelWidth(plot_area_width_guess,
                                                                      plot_area_height);

    // Compute the final y-tick area width
    const uint ytick_area_width = max_ytick_label_width + tick_width + 2*tick_area_y_->getXMargin();

    // Compute the final plot area width
    const uint plot_area_width = geom.width() - ytick_area_width;

    // Update the last y-tick area width
    tick_area_y_->last_width_ = ytick_area_width;

    // Set the geometries of the tick areas, the plot area and the UI
    tick_area_x_->setGeometry(ytick_area_width,geom.height()-x_ticks_area_height,
                              geom.width()-ytick_area_width,x_ticks_area_height);
    tick_area_y_->setGeometry(0,0,ytick_area_width,geom.height()-x_ticks_area_height);
    plot_area_->setGeometry(ytick_area_width,0,geom.width()-ytick_area_width,
                            geom.height()-x_ticks_area_height);
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
