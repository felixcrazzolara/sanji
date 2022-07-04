#include <QPainter>
#include <QFontMetrics>
#include "VTicksArea.hpp"
#include "Figure.hpp"
#include <stdio.h>

namespace sanji_ {

VTicksArea::VTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent) :
    TicksArea(limits_info,plot_area,parent)
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
    const auto [xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px] = limits_info_->getScalingsAndLimits(plot_geom.width(),plot_geom.height());

    /* Determine the tick locations */
    // Determine the minimum number of pixels per tick
    QFontMetrics fm(QFont("Monospace",10));
    const uint min_pixel_per_tick = fm.boundingRect(QString("0")).height();

    // Minimize 'num_pixel_per_tick' while making sure that 'num_pixel_per_tick' >= 'min_pixel_per_tick'
    int num_pixel_per_tick = -1;
    const double mult_mult = 2.0;
    double mult            = 1.0;
    double mult10          = 1.0;
    do {
        const int ymin     = std::ceil(mult*mult10*yplot_min);
        const int ymax     = std::floor(mult*mult10*yplot_max);
        if (ymax-ymin+1 != 0) num_pixel_per_tick = geom.height()/(ymax-ymin+1);
        mult              *= mult_mult;
        if (mult >= 10.0) {
            mult = 1.0;
            mult10 *= 10.0;
        }
    } while (num_pixel_per_tick == -1 || num_pixel_per_tick >= min_pixel_per_tick);
    mult = mult*mult10;

    //--> At this point it holds that 'num_pixel_per_tick' < 'min_pixel_per_tick'

    while (num_pixel_per_tick < min_pixel_per_tick) {
        mult              /= mult_mult;
        const int ymin     = std::ceil(mult*yplot_min);
        const int ymax     = std::floor(mult*yplot_max);
        num_pixel_per_tick = geom.height()/(ymax-ymin+1);
    }

    //--> At this point it holds that 'num_pixel_per_tick' >= 'min_pixel_per_tick'

    const uint num_digits = 5;

    // Plot the ticks
    painter.setPen(QPen(QColor(0,0,0)));
    char* chr_buffer  = new char[20];
    const double ymin = std::ceil(mult*yplot_min)/mult;
    const double ymax = std::floor(mult*yplot_max)/mult;
    const auto   toYCoord = [yplot_max,dy_to_px](const double y)->int {
        return (yplot_max-y)*dy_to_px;
    };
    double y        = ymin;
    int label_count = 0;
    do {
        if (y == 0.0) {
            sprintf(chr_buffer,"0\n");
        } else {
            const double exp = std::log10(std::abs(y));
            if (exp < num_digits && exp >= 0.0) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-static_cast<uint>(std::floor(exp))-1)+"f\n").c_str(),static_cast<double>(y));
            } else if (exp < 0.0 && exp >= num_digits-1) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"f\n").c_str(),static_cast<double>(y));
            } else {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"e\n").c_str(),static_cast<double>(y));
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
