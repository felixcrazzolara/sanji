#include <QFontMetrics>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "RenderArea.hpp"

namespace sanji_ {

// TODO: Put these somewhere else
static constexpr uint PLOT_UI_WIDTH      = 90;
static constexpr uint MIN_PLOT_UI_WIDTH  = 30;
static constexpr uint PLOT_UI_HEIGHT     = 21;
static constexpr uint MIN_PLOT_UI_HEIGHT = 7;

RenderArea::RenderArea(const LineDataWrapper*  line_data,
                       const ArrowDataWrapper* arrow_data,
                             LimitsInfo&       limits_info,
                             QWidget*          parent) :
    QWidget(parent),
    line_data_(line_data),
    arrow_data_(arrow_data)
{
    // Logically split the plot in two sides. The left one with the y-tick area and the right one with the
    // actual plot at the top and the x-tick area at the bottom.
    QHBoxLayout* h_layout = new QHBoxLayout(this);
    QWidget*     l_side   = new QWidget(this);
    QWidget*     r_side   = new QWidget(this);
    h_layout->addWidget(l_side);
    h_layout->addWidget(r_side);
    h_layout->setContentsMargins(0,0,0,0);
    h_layout->setSpacing(0);

    // Instantiate the plot area
    plot_area_ = new PlotArea(line_data, arrow_data, &limits_info, r_side);

    // On the left side, put the y-tick area at the top and some stretch below
    QVBoxLayout* l_v_layout = new QVBoxLayout(l_side);
    tick_area_y_ = new VTicksArea(&limits_info, plot_area_, l_side);
    l_v_layout->addWidget(tick_area_y_);
    l_v_layout->addStretch();
    l_v_layout->setContentsMargins(0,0,0,0);
    l_v_layout->setSpacing(0);

    // On the right side, put the plot area at the top and the x-tick area below
    QVBoxLayout* r_v_layout = new QVBoxLayout(r_side);
    tick_area_x_ = new HTicksArea(&limits_info, plot_area_, r_side);
    r_v_layout->addWidget(plot_area_);
    r_v_layout->addWidget(tick_area_x_);
    r_v_layout->setContentsMargins(0,0,0,0);
    r_v_layout->setSpacing(0);

    // Instantiate the plot UI on top of the plot area
    plot_ui_ = new PlotUI(&limits_info, this);

    // Show the render area
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

void RenderArea::updateContent() {
    // Nothing to do if there is no data
    if (!(line_data_->hasData() || arrow_data_->hasData())) return;

    // TODO: Put these settings somewhere else
    const uint num_digits  = 5;
    const uint tick_height = 11;
    const uint tick_width  = 11;

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

    uint plot_area_height;
    uint xtick_area_height;
    // If the render area is so small that not even the x-tick area would fit in, just the plot area is
    // shown.
    if (geom.height() <= tick_area_x_->getHeight()) {
        plot_area_height  = geom.height();
        xtick_area_height = 0;
    } else {
        plot_area_height  = geom.height() - tick_area_x_->getHeight();
        xtick_area_height = tick_area_x_->getHeight();
    }
    // It is only necessary to determine the width required by the y-tick area if it has a minimal
    // height.
    uint ytick_area_width;
    if (plot_area_height >= tick_area_y_->getMinHeight()) {
        const QFontMetrics fm = QFontMetrics(QFont("Monospace",10));

        // Estimate the plot area width
        uint plot_area_width_guess;
        if (tick_area_y_->last_width_ == -1) {
            char* buf = new char[num_digits];
            for (size_t i = 0; i < num_digits-1; ++i) buf[i] = '0';
            buf[num_digits-1] = '\0';
            plot_area_width_guess = geom.width() - fm.boundingRect(QString::fromStdString(
                "-0."+std::string(buf)+"e-100")).width() - tick_width - 2*tick_area_y_->getXMargin();
            delete[] buf;
        } else {
            plot_area_width_guess = geom.width() - tick_area_y_->last_width_;
        }

        // Compute the maximum y-tick label width based on the estimate of the size of the plot area
        const uint max_ytick_label_width = tick_area_y_->getMaxLabelWidth(plot_area_width_guess,
                                                                          plot_area_height);

        // Compute the final y-tick area width
        ytick_area_width = max_ytick_label_width + tick_width;
    }
    // Otherwise, just set the y-tick area width to zero, it's of no use anyway.
    else {
        ytick_area_width = 0;
    }

    // Compute the final plot area width
    const uint plot_area_width = geom.width() - ytick_area_width;

    // Update the last y-tick area width
    tick_area_y_->last_width_ = ytick_area_width;

    // Set the geometries of the tick areas and the plot area
    tick_area_x_->setSizeHint(QSize(plot_area_width,xtick_area_height));
    tick_area_y_->setSizeHint(QSize(ytick_area_width,geom.height()-xtick_area_height));
    plot_area_->setSizeHint(QSize(plot_area_width,geom.height()-xtick_area_height));
    plot_area_->updateGeometry();
    tick_area_x_->updateGeometry();
    tick_area_y_->updateGeometry();

    // Position the UI correctly
    if (plot_area_width >= MIN_PLOT_UI_WIDTH && geom.height()-xtick_area_height >= MIN_PLOT_UI_HEIGHT) {
        const uint plot_ui_width  = std::min(PLOT_UI_WIDTH, plot_area_width);
        const uint plot_ui_height = std::min(PLOT_UI_HEIGHT, geom.height()-xtick_area_height);

        plot_ui_->setGeometry(geom.width()-plot_ui_width, 0, plot_ui_width, plot_ui_height);
        plot_ui_->setVisible(true);
    } else {
        plot_ui_->setVisible(false);
    }
}

void RenderArea::resizeEvent(QResizeEvent* event) {
    updateContent();
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
