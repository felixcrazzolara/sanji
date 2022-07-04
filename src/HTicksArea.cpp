#include <QPainter>
#include <QFontMetrics>
#include <stdio.h>
#include <cmath>
#include "HTicksArea.hpp"
#include "Figure.hpp"

namespace sanji_ {

HTicksArea::HTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent) :
    TicksArea(limits_info,plot_area,parent)
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

    const uint num_digits_after_comma = 5;

    // Draw a thin horizontal line first
    painter.setPen(QPen(QColor(171,171,171)));
    painter.drawLine(0,(tick_height-1)/2,geom.width(),(tick_height-1)/2);

    // Determine the axis limits
    const QRect plot_geom = plot_area_->geometry();
    const auto [xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px] = limits_info_->getScalingsAndLimits(plot_geom.width(),plot_geom.height());

    /* Determine the tick locations */
    // Determine the minimum number of pixels per tick
    QFontMetrics fm(QFont("Monospace",10));
    char* buf                     = new char[num_digits_after_comma+1];
    for (uint i = 0; i < num_digits_after_comma-1; ++i) buf[i] = '0';
    buf[num_digits_after_comma]   = '\0';
    const uint min_pixel_per_tick = fm.boundingRect(QString::fromStdString("-0."+std::string(buf)+"e-100")).width();
    delete[] buf;

    // Minimize 'num_pixel_per_tick' while making sure that 'num_pixel_per_tick' >= min_pixel_per_tick
    int num_pixel_per_tick = -1;
    const double mult_mult = 2.0;
    double mult            = 1.0;
    double mult10          = 1.0;
    do {
        const int xmin     = std::ceil(mult*mult10*xplot_min);
        const int xmax     = std::floor(mult*mult10*xplot_max);
        if (xmax-xmin+1 != 0) num_pixel_per_tick = geom.width()/(xmax-xmin+1);
        mult              *= mult_mult;
        if (mult >= 10.0) {
            mult    = 1.0;
            mult10 *= 10.0;
        }
    } while (num_pixel_per_tick == -1 || num_pixel_per_tick >= min_pixel_per_tick);
    mult = mult*mult10;

    //--> At this point it holds that num_pixel_per_tick < min_pixel_per_tick

    while (num_pixel_per_tick < min_pixel_per_tick) {
        mult              /= mult_mult;
        const int xmin     = std::ceil(mult*xplot_min);
        const int xmax     = std::floor(mult*xplot_max);
        num_pixel_per_tick = geom.width()/(xmax-xmin+1);
    }

    //--> At this point it holds that num_pixel_per_tick >= min_pixel_per_tick

    // Plot the ticks
    painter.setPen(QPen(QColor(0,0,0)));
    char* chr_buffer      = new char[20];
    const double xmin     = std::ceil(mult*xplot_min)/mult;
    const double xmax     = std::floor(mult*xplot_max)/mult;
    const auto   toXCoord = [xplot_min,dx_to_px](const double x)->int {
        return (x-xplot_min)*dx_to_px;
    };
    double x            = xmin;
    int label_count     = 0;
    do {
        if (x == 0.0) {
            sprintf(chr_buffer,"0\n");
        } else {
            // TODO: Check this again
            const double exp = std::log10(std::abs(x));
            if (exp < num_digits_after_comma && exp >= 0.0) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits_after_comma-static_cast<uint>(std::floor(exp))-1)+"f\n").c_str(),x);
            } else if (exp < 0.0 && exp >= num_digits_after_comma-1) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits_after_comma-1)+"f\n").c_str(),x);
            } else {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits_after_comma-1)+"e\n").c_str(),x);
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
