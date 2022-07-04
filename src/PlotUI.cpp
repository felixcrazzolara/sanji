#include <QPainter>
#include <QMouseEvent>
#include "PlotUI.hpp"

#include <iostream> // TODO: Remove this later

namespace sanji_ {

PlotUI::PlotUI(LimitsInfo* limits_info, QWidget* parent) :
    QWidget(parent),
    background_color_{255,255,255},
    limits_info_{limits_info}
{}

void PlotUI::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color_);
    painter.drawRect(0,0,geom.width(),geom.height());

    // Draw separating lines
    painter.setPen(QColor(158,158,158));
    painter.drawLine(  PLOT_UI_WIDTH/3,0,  PLOT_UI_WIDTH/3,PLOT_UI_HEIGHT);
    painter.drawLine(2*PLOT_UI_WIDTH/3,0,2*PLOT_UI_WIDTH/3,PLOT_UI_HEIGHT);

    // Draw the icons into the buttons
    // TODO: Implement this...
}

void PlotUI::mousePressEvent(QMouseEvent *event) {
    // Set the press index
    press_index_ = getButtonIndex(event->pos());
}

void PlotUI::mouseReleaseEvent(QMouseEvent *event) {
    // Get the button index
    int button_idx = getButtonIndex(event->pos());

    // Handle the event
    if (button_idx == press_index_) {
        switch (button_idx) {
            case 0:  limits_info_->decrease_hist_index_if_possible(); break;
            case 1:  limits_info_->increase_hist_index_if_possible(); break;
            case 2:  limits_info_->reset_hist_index();                break;
            default: break;
        }

        // Update the entire plot
        parentWidget()->update();
    }
}

int PlotUI::getButtonIndex(const QPoint& p) const {
    if (p.x() < 0 || p.y() < 0 || p.x() >= PLOT_UI_WIDTH || p.y() >= PLOT_UI_HEIGHT) return -1;
    else if (p.x() < PLOT_UI_WIDTH/3) return 0;
    else if (p.x() < 2*PLOT_UI_WIDTH/3) return 1;
    else return 2;
}

};
