#include <QPainter>
#include <QFontMetrics>
#include "VTicksArea.hpp"
#include "Figure.hpp"
#include <stdio.h>

namespace sanji_ {

VTicksArea::VTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area) :
    TicksArea(limits_info,plot_area)
{}

void VTicksArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color_);
    painter.drawRect(0,0,geom.width(),geom.height());

    const uint tick_width = 11;

    // Draw a thin vertical line first
    painter.setPen(QPen(QColor(171,171,171)));
    painter.drawLine(geom.width()-(tick_width+1)/2,0,geom.width()-(tick_width+1)/2,geom.height());

    // Determine the axis limits
    const QRect plot_geom = plot_area_->geometry();
    double x_to_px        = plot_geom.width()/(limits_info_->xmax-limits_info_->xmin);
    double y_to_px        = plot_geom.height()/(limits_info_->ymax-limits_info_->ymin);
    double ymin_          = limits_info_->ymin;
    double ymax_          = limits_info_->ymax;
    if (limits_info_->axes_ratio == LimitsInfo::AXES_RATIO::EQUAL) {
        if (x_to_px < y_to_px) {
            y_to_px = x_to_px;
            ymin_   = limits_info_->ymin-std::max(geom.height()/y_to_px-(limits_info_->ymax-limits_info_->ymin),0.0)/2.0;
            ymax_   = limits_info_->ymax+std::max(geom.height()/y_to_px-(limits_info_->ymax-limits_info_->ymin),0.0)/2.0;
        }
    }

    // Determine the tick locations
    uint num_pixel_per_tick = 100;
    uint mult               = 1;
    while (num_pixel_per_tick > 75u) {
        const int ymin     = std::ceil(mult*ymin_);
        const int ymax     = std::floor(mult*ymax_);
        num_pixel_per_tick = geom.height()/(ymax-ymin+1);
        mult              *= 10;
    }
    mult /= 10;

    const uint num_digits = 5;

    // Plot the ticks
    QFontMetrics fm(QFont("Monospace",10));
    painter.setPen(QPen(QColor(0,0,0)));
    char* chr_buffer      = new char[20];
    const double ymin     = std::ceil(mult*ymin_)/mult;
    const double ymax     = std::floor(mult*ymax_)/mult;
    const auto   toYCoord = [ymax_,y_to_px](const double y)->int {
        return (ymax_-y)*y_to_px;
    };
    double y            = ymin;
    int label_count     = 0;
    do {
        if (y == 0.0) {
            sprintf(chr_buffer,"0\n");
        } else {
            const double exp = std::log10(std::abs(y));
            if (exp < num_digits && exp >= 0.0) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-static_cast<uint>(std::floor(exp))-1)+"f\n").c_str(),y);
            } else if (exp < 0.0 && exp >= num_digits-1) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"f\n").c_str(),y);
            } else {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"e\n").c_str(),y);
            }
        }
        const int   ycoord       = toYCoord(y);
        const QRect label_geom   = fm.boundingRect(QString(chr_buffer));
        const int   label_height = label_geom.height();
        painter.drawLine(geom.width()-tick_width,ycoord,geom.width()-1,ycoord);
        if (label_height % 2 == 0) {
            if (ycoord-(label_height-2)/2 >= 0 && ycoord+(label_height-2)/2 < geom.height()) {
                if (!(tick_labels_.size() > label_count))
                    tick_labels_.push_back(new QLabel(this));
                tick_labels_[label_count]->setGeometry(0,ycoord-(label_height-2)/2,label_geom.width(),label_height);
                tick_labels_[label_count]->setFont(QFont("Monospace",10));
                tick_labels_[label_count]->setText(QString(chr_buffer));
                tick_labels_[label_count++]->show();
            }
        } else {
            if (ycoord-(label_height-1)/2 >= 0 && ycoord+(label_height-1)/2 < geom.height()) {
                if (!(tick_labels_.size() > label_count))
                    tick_labels_.push_back(new QLabel(this));
                tick_labels_[label_count]->setGeometry(0,ycoord-(label_height-1)/2,label_geom.width(),label_height);
                tick_labels_[label_count]->setFont(QFont("Monospace",10));
                tick_labels_[label_count]->setText(QString(chr_buffer));
                tick_labels_[label_count++]->show();
            }
        }
        y = (y*mult + 1)/mult;
    } while (y <= ymax);
    for (int i = tick_labels_.size()-1; i >= label_count; --i) {
        delete tick_labels_[i];
        tick_labels_.pop_back();
    }
    delete[] chr_buffer;
}

};
