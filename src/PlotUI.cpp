/* 
 * Author: Felix Crazzolara
 */ 
#include <QPainter>
#include <QMouseEvent>

#include "PlotUI.hpp"

namespace sanji_ {

PlotUI::PlotUI(LimitsInfo* limits_info, QWidget* parent) :
    QFrame(parent),
    background_color_{255,255,255},
    limits_info_{limits_info}
{
    // Add a border
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(1);
}

void PlotUI::paintEvent(QPaintEvent* event) {
    // Fetch the geometry of the this widget
    const QRect geom   = geometry();
    const uint  width  = geom.width();
    const uint  height = geom.height();

    // Creater a painter
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color_);
    painter.drawRect(0,0,geom.width(),geom.height());

    // Draw separating lines
    painter.setPen(QColor(158,158,158));
    painter.drawLine(  width/3,0,  width/3,height);
    painter.drawLine(2*width/3,0,2*width/3,height);

    // Update the positions of the button separation lines
    limit_x1_ =   width/3;
    limit_x2_ = 2*width/3;

    /**** Draw the icons into the buttons ****/
    /* Draw the home button */
    const uint home_cell_width      = width/3;
    const uint roof_tip_x           = 0.5*home_cell_width;
    const uint roof_tip_y           = 0.2*height;
    const uint roof_start_x         = roof_tip_x - static_cast<uint>(0.25*home_cell_width);
    const uint roof_end_x           = roof_tip_x + static_cast<uint>(0.25*home_cell_width);
    const uint roof_bottom_y        = 0.45*height;
    const uint house_bottom_start_x = roof_start_x + static_cast<uint>(0.15*(roof_end_x-roof_start_x));
    const uint house_bottom_end_x   = roof_end_x   - static_cast<uint>(0.15*(roof_end_x-roof_start_x));
    const uint house_bottom_y       = 0.8*height;

    // Draw the roof
    painter.setPen(QColor(100,100,100));
    painter.drawLine(roof_start_x, roof_bottom_y,
                     roof_tip_x,   roof_tip_y);
    painter.drawLine(roof_end_x,   roof_bottom_y,
                     roof_tip_x,   roof_tip_y);
    painter.drawLine(roof_start_x, roof_bottom_y,
                     roof_end_x,   roof_bottom_y);

    // Draw lower part of the house
    painter.drawLine(house_bottom_start_x, roof_bottom_y+1,
                     house_bottom_start_x, house_bottom_y-1);
    painter.drawLine(house_bottom_end_x,   roof_bottom_y+1,
                     house_bottom_end_x,   house_bottom_y-1);
    painter.drawLine(house_bottom_start_x, house_bottom_y,
                     house_bottom_end_x,   house_bottom_y);

    /* Draw the backward button */
    const uint backward_cell_width = 2*width/3 - width/3 - 1;
    const uint icon_height         = 1+2*static_cast<uint>(height*0.25);
    const uint icon_center_y       = 0.5*height;
    const uint icon_top_y          = icon_center_y - (icon_height-1)/2;
    const uint icon_bottom_y       = icon_center_y + (icon_height-1)/2;
          uint icon_left_x         = width/3 + static_cast<uint>(backward_cell_width*0.5)
                                             - static_cast<uint>(backward_cell_width*0.15);
          uint icon_right_x        = width/3 + static_cast<uint>(backward_cell_width*0.5)
                                             + static_cast<uint>(backward_cell_width*0.15);

    painter.drawLine(icon_right_x, icon_top_y,
                     icon_left_x,  icon_center_y);
    painter.drawLine(icon_left_x,  icon_center_y,
                     icon_right_x, icon_bottom_y);

    /* Draw the forward button */
    const uint forward_cell_width = geom.width() - 2*width/3 - 1;
               icon_left_x         = 2*width/3 + static_cast<uint>(forward_cell_width*0.5)
                                               - static_cast<uint>(forward_cell_width*0.15);
               icon_right_x        = 2*width/3 + static_cast<uint>(forward_cell_width*0.5)
                                               + static_cast<uint>(forward_cell_width*0.15);

    painter.drawLine(icon_left_x,  icon_top_y,
                     icon_right_x, icon_center_y);
    painter.drawLine(icon_right_x, icon_center_y,
                     icon_left_x,  icon_bottom_y);

    // Make sure that the border is drawn
    QFrame::paintEvent(event);
}

void PlotUI::mousePressEvent(QMouseEvent *event) {
    // Set the press index
    press_idx_ = getButtonIndex(event->pos());
}

void PlotUI::mouseReleaseEvent(QMouseEvent *event) {
    // Get the button index
    const ButtonIndex release_idx = getButtonIndex(event->pos());

    // Handle the event
    if (release_idx == press_idx_) {
        switch (release_idx) {
            case ButtonIndex::HOME:     limits_info_->reset_hist_index();                break;
            case ButtonIndex::BACKWARD: limits_info_->decrease_hist_index_if_possible(); break;
            case ButtonIndex::FORWARD:  limits_info_->increase_hist_index_if_possible(); break;
            default: break;
        }

        // Update the entire plot
        parentWidget()->update();
    }
}

PlotUI::ButtonIndex PlotUI::getButtonIndex(const QPoint& p) const {
    // Get the dimensions of the plot UI
    const uint width  = geometry().width();
    const uint height = geometry().height();

    // Check if the clicked point is within on the home button
    if (p.x() >= 0 && p.x() < limit_x1_ && p.y() >= 0 && p.y() < height) {
        return ButtonIndex::HOME;
    } else if (p.x() > limit_x1_ && p.x() < limit_x2_ && p.y() >= 0 && p.y() < height) {
        return ButtonIndex::BACKWARD;
    } else if (p.x() > limit_x2_ && p.x() < width && p.y() >= 0 && p.y() < height) {
        return ButtonIndex::FORWARD;
    } else {
        return ButtonIndex::INVALID;
    }
}

};
