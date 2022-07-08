#include <QPainter>
#include <QFontMetrics>

#include <stdio.h>
#include <cmath>

#include "HTicksArea.hpp"
#include "Figure.hpp"

namespace sanji_ {

static constexpr uint FONT_SIZE   = 10;
static constexpr uint TICK_HEIGHT = 11;
static constexpr uint Y_MARGIN    = 2;

HTicksArea::HTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent) :
    TicksArea(limits_info,plot_area,parent)
{}

uint HTicksArea::getHeight() const {
    return QFontMetrics(QFont("Monospace",FONT_SIZE)).height() + 2*Y_MARGIN + TICK_HEIGHT;
}

void HTicksArea::setSizeHint(const QSize& size_hint) {
    size_hint_ = size_hint;
}

QSize HTicksArea::sizeHint() const {
    return size_hint_;
}

void HTicksArea::paintEvent(QPaintEvent* event) {
    // Miscellaneous
    QFontMetrics fm(QFont("Monospace",FONT_SIZE));
    QPainter     painter(this);

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color_);
    painter.drawRect(0,0,geom.width(),geom.height());

    // TODO: Put these settings somewhere else
    const uint num_digits_after_comma = 5;

    // Draw a thin horizontal line first
    painter.setPen(QPen(QColor(171,171,171)));
    painter.drawLine(0,(TICK_HEIGHT-1)/2,geom.width()-1,(TICK_HEIGHT-1)/2);

    // Determine the axis limits
    const QRect plot_geom = plot_area_->geometry();
    const auto [xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px] = 
        limits_info_->getScalingsAndLimits(plot_geom.width(),plot_geom.height());

    /* Determine the tick locations */
    double mult = 1.0;
    // If there are no data points or they are aligned on a vertical line, just add a single tick at 0 or
    // at the value of the vertical line, respectively.
    if (xplot_min == xplot_max) {
        // Nothing to do
    }
    // Else, find a good number of ticks such that tick labels don't overlap, but the ticks are sufficiently
    // dense.
    else {
        // Determine the minimum number of pixels per tick
        char* buf = new char[num_digits_after_comma+1];
        for (size_t i = 0; i < num_digits_after_comma; ++i) {
            if (i < num_digits_after_comma-1) {
                buf[i] = '0';
            } else {
                buf[i] = '\0';
            }
        }
        const uint min_pixel_per_tick = fm.boundingRect(
            QString::fromStdString("-0."+std::string(buf)+"e-100")).width();
        delete[] buf;

        // Minimize num_pixel_per_tick just below num_pixel_per_tick
        int num_pixel_per_tick = -1;
        const double mult_mult = 2.0;
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
        assert(num_pixel_per_tick < min_pixel_per_tick);

        // Decrease mult until num_pixel_per_tick >= min_pixel_per_tick
        while (num_pixel_per_tick < min_pixel_per_tick) {
            mult              /= mult_mult;
            const int xmin     = std::ceil(mult*xplot_min);
            const int xmax     = std::floor(mult*xplot_max);
            num_pixel_per_tick = geom.width()/(xmax-xmin+1);
        }

        //--> At this point it holds that num_pixel_per_tick >= min_pixel_per_tick
        assert(num_pixel_per_tick >= min_pixel_per_tick);
    }

    /* Plot the ticks */
    // Prepare a pen to draw the ticks and memory for the tick labels
    painter.setPen(QPen(QColor(0,0,0)));
    char* chr_buffer = new char[20];

    // Determine the values of the leftmost and the rightmost tick
    double xmin, xmax;
    if (xplot_min == xplot_max) {
        xmin = xmax = xplot_min;
    } else {
        xmin = std::ceil(mult*xplot_min)/mult;
        xmax = std::floor(mult*xplot_max)/mult;
    }

    // Create a lambda to convert x-axis values to horizontal pixel coordinates
    const auto toXCoord = [xplot_min,xplot_max,dx_to_px,x_mid=plot_geom.width()/2](const double x)->int {
        if (xplot_min == xplot_max) {
            assert(x == xplot_min);
            return x_mid;
        } else {
            return (x-xplot_min)*dx_to_px;
        }
    };

    // Iterate through the x-axis values at the tick positions
    double x           = xmin;
    int    label_count = 0;
    do {
        // Convert the x-axis value to a string
        if (x == 0.0) {
            sprintf(chr_buffer,"0\n");
        } else {
            // TODO: Check this again
            const double exp = std::log10(std::abs(x));
            if (exp < num_digits_after_comma && exp >= 0.0) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits_after_comma-static_cast<uint>(std::floor(exp))-1)+"g\n").c_str(),x);
            } else if (exp < 0.0 && std::abs(exp) < num_digits_after_comma-1) {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits_after_comma-1)+"g\n").c_str(),x);
            } else {
                sprintf(chr_buffer,std::string("%."+std::to_string(num_digits_after_comma-1)+"e\n").c_str(),x);
            }
        }

        // Determine the width and the height of the tick label
        const QRect text_geom   = fm.boundingRect(QString(chr_buffer));
        const int   text_width  = text_geom.width();
        const uint  text_height = text_geom.height();

        // Draw the vertical tick line
        const int xcoord = toXCoord(x);
        painter.drawLine(xcoord,0,xcoord,TICK_HEIGHT-1);

        // Draw the label only if there's enough space
        if (xcoord-text_width/2 >= 0 && xcoord+text_width/2 < geom.width()) {
            // Check if an old tick label can be reused or if a new one must be created
            if (tick_labels_.size() <= label_count) {
                tick_labels_.push_back(new QLabel(this));
                tick_labels_.back()->setFont(QFont("Monospace",FONT_SIZE));
            }

            // Configure the label
            tick_labels_[label_count]->setText(QString(chr_buffer));
            tick_labels_[label_count]->setAlignment(Qt::AlignHCenter);
            tick_labels_[label_count]->setGeometry(xcoord-text_width/2,TICK_HEIGHT,
                                                   text_width,text_height+2*Y_MARGIN);
            tick_labels_[label_count++]->show();
        }

        // Step to the next x-axis value considered
        x = (x*mult + 1.0) / mult;
    } while (x <= xmax);

    // Delete tick labels that are not used anymore
    for (int i = tick_labels_.size()-1; i >= label_count; --i) {
        delete tick_labels_[i];
        tick_labels_.pop_back();
    }

    // Free memory
    delete[] chr_buffer;
}

};
