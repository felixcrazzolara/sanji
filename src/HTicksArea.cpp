#include <QPainter>
#include <QFontMetrics>
#include <stdio.h>
#include <cmath>
#include "HTicksArea.hpp"
#include "Figure.hpp"

namespace sanji_ {

HTicksArea::HTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area) :
    TicksArea(limits_info,plot_area)
{}

void HTicksArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color_);
    painter.drawRect(0,0,geom.width(),geom.height());

    const uint tick_height = 11;

    const uint num_digits = 5;

    // Draw a thin horizontal line first
    painter.setPen(QPen(QColor(171,171,171)));
    painter.drawLine(0,(tick_height-1)/2,geom.width(),(tick_height-1)/2);

    // Determine the axis limits
    const QRect plot_geom = plot_area_->geometry();
    double x_to_px        = plot_geom.width()/(limits_info_->xmax-limits_info_->xmin);
    double y_to_px        = plot_geom.height()/(limits_info_->ymax-limits_info_->ymin);
    double xmin_          = limits_info_->xmin;
    double xmax_          = limits_info_->xmax;
    if (limits_info_->axes_ratio == LimitsInfo::AXES_RATIO::EQUAL) {
        if (x_to_px >= y_to_px) {
            x_to_px = y_to_px;
            xmin_   = limits_info_->xmin-std::max(geom.width()/x_to_px-(limits_info_->xmax-limits_info_->xmin),0.0)/2.0;
            xmax_   = limits_info_->xmax+std::max(geom.width()/x_to_px-(limits_info_->xmax-limits_info_->xmin),0.0)/2.0;
        }
    }

    // Determine the tick locations
    const double mult_mult        = 2.0;
    QFontMetrics fm(QFont("Monospace",10));
    uint num_pixel_per_tick       = 1000;
    double mult                   = 1.0;
    char* buf                     = new char[num_digits];
    for (uint i = 0; i < num_digits-1; ++i) buf[i] = '0';
    buf[num_digits-1]             = '\0';
    const uint min_pixel_per_tick = fm.boundingRect(QString::fromStdString("-0."+std::string(buf)+"e-100")).width();
    delete[] buf;
    while (num_pixel_per_tick > min_pixel_per_tick) {
        const int xmin     = std::ceil(mult*xmin_);
        const int xmax     = std::floor(mult*xmax_);
        if (xmax-xmin+1 != 0) num_pixel_per_tick = geom.width()/(xmax-xmin+1);
        mult              *= mult_mult;
    }
    mult /= mult_mult;
    while (num_pixel_per_tick < min_pixel_per_tick) {
        mult              /= mult_mult;
        const int xmin     = std::ceil(mult*xmin_);
        const int xmax     = std::floor(mult*xmax_);
        num_pixel_per_tick = geom.width()/(xmax-xmin+1);
    }

    // Plot the ticks
    painter.setPen(QPen(QColor(0,0,0)));
    char* chr_buffer      = new char[20];
    const double xmin     = std::ceil(mult*xmin_)/mult;
    const double xmax     = std::floor(mult*xmax_)/mult;
    const auto   toXCoord = [xmin_,x_to_px](const double x)->int {
        return (x-xmin_)*x_to_px;
    };
    double x            = xmin;
    int label_count     = 0;
    do {
        if (x == 0.0) {
            sprintf(chr_buffer,"0\n");
        } else {
            const double exp = std::log10(std::abs(x));
            if (exp < num_digits && exp >= 0.0) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-static_cast<uint>(std::floor(exp))-1)+"f\n").c_str(),x);
            } else if (exp < 0.0 && exp >= num_digits-1) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"f\n").c_str(),x);
            } else {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"e\n").c_str(),x);
            }
        }
        const int xcoord        = toXCoord(x);
        const QRect label_geom  = fm.boundingRect(QString(chr_buffer));
        const int label_width   = label_geom.width();
        const uint label_height = label_geom.height();
        painter.drawLine(xcoord,0,xcoord,tick_height-1);
        if (label_width % 2 == 0) {
            if (xcoord-(label_width-2)/2 >= 0 && xcoord+(label_width-2)/2 < geom.width()) {
                if (!(tick_labels_.size() > label_count))
                    tick_labels_.push_back(new QLabel(this));
                tick_labels_[label_count]->setGeometry(xcoord-(label_width-2)/2,tick_height+1,label_width,label_height);
                tick_labels_[label_count]->setText(QString(chr_buffer));
                tick_labels_[label_count++]->show();
            }
        } else {
            if (xcoord-(label_width-1)/2 >= 0 && xcoord+(label_width-1)/2 < geom.width()) {
                if (!(tick_labels_.size() > label_count))
                    tick_labels_.push_back(new QLabel(this));
                tick_labels_[label_count]->setGeometry(xcoord-(label_width-1)/2,tick_height+1,label_width,label_height);
                tick_labels_[label_count]->setFont(QFont("Monospace",10));
                tick_labels_[label_count]->setText(QString(chr_buffer));
                tick_labels_[label_count++]->show();
            }
        }
        x = (x*mult + 1)/mult;
    } while (x <= xmax);
    for (int i = tick_labels_.size()-1; i >= label_count; --i) {
        delete tick_labels_[i];
        tick_labels_.pop_back();
    }
    delete[] chr_buffer;
}

};
