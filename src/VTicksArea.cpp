#include "Figure.hpp"
#include "VTicksArea.hpp"

#include <QFontMetrics>
#include <QPainter>

#include <stdio.h>

#include <iostream>

namespace sanji_ {

// TODO: Implement this differently
static constexpr uint FONT_SIZE = 10;
static constexpr uint X_MARGIN  = 2;
static constexpr uint Y_MARGIN  = 2;

VTicksArea::VTicksArea(const LimitsInfo* limits_info, const PlotArea* plot_area, QWidget* parent) :
    TicksArea(limits_info,plot_area,parent),
    last_width_(-1)
{}

uint VTicksArea::getXMargin() const {
    return X_MARGIN;
}

uint VTicksArea::getYMargin() const {
    return Y_MARGIN;
}

uint VTicksArea::getMinHeight() const {
    return QFontMetrics(QFont("Monospace",FONT_SIZE)).height() + 2*getYMargin();
}

void VTicksArea::setSizeHint(const QSize& size_hint) {
    size_hint_ = size_hint;
}

QSize VTicksArea::sizeHint() const {
    return size_hint_;
}

void VTicksArea::paintEvent(QPaintEvent* event) {
    // Miscellaneous
    QPainter     painter(this);
    QFontMetrics fm(QFont("Monospace",FONT_SIZE));

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

    // Don't paint anything if the height is smaller than the minimum height
    if (geom.height() < getMinHeight()) return;

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color_);
    painter.drawRect(0,0,geom.width(),geom.height());

    // TODO: Put this setting somewhere else
    const uint tick_width = 11;

    // Draw a thin vertical line first
    painter.setPen(QPen(QColor(171,171,171)));
    painter.drawLine(geom.width()-(tick_width+1)/2,0,geom.width()-(tick_width+1)/2,geom.height()-1);

    // Determine the axis limits
    const QRect plot_geom = plot_area_->geometry();
    const auto [xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px] =
        limits_info_->getScalingsAndLimits(plot_geom.width(),plot_geom.height());

    /* Determine the tick locations */
    double mult = 1.0;
    // If there are no data points or they are aligned on a horizontal line, just add a single tick at 0 or
    // at the value of the horizontal line, respectively.
    if (yplot_min == yplot_max) {
        // Nothing to do
    }
    // Else, find a good number of ticks such that tick labels don't overlap, but the ticks are sufficiently
    // dense.
    else {
        // Determine the minimum number of pixels per tick
        const uint min_pixel_per_tick = fm.height();

        // Minimize num_pixel_per_tick just below num_pixel_per_tick
        int num_pixel_per_tick = -1;
        const double mult_mult = 2.0;
        double mult10          = 1.0;
        do {
            const int ymin = std::ceil(mult*mult10*yplot_min);
            const int ymax = std::floor(mult*mult10*yplot_max);

            if (ymax-ymin+1 != 0) num_pixel_per_tick = geom.height()/(ymax-ymin+1);
            mult              *= mult_mult;
            if (mult >= 10.0) {
                mult    = 1.0;
                mult10 *= 10.0;
            }
        } while (num_pixel_per_tick == -1 || num_pixel_per_tick >= min_pixel_per_tick);
        mult = mult*mult10;

        //--> At this point it holds that num_pixel_per_tick < min_pixel_per_tick
        assert(num_pixel_per_tick < min_pixel_per_tick);

        // Decrease mult until 
        while (num_pixel_per_tick < min_pixel_per_tick) {
            mult              /= mult_mult;
            const int ymin     = std::ceil(mult*yplot_min);
            const int ymax     = std::floor(mult*yplot_max);
            num_pixel_per_tick = geom.height() / (ymax-ymin+1);
        }

        //--> At this point it holds that num_pixel_per_tick >= min_pixel_per_tick
        assert(num_pixel_per_tick >= min_pixel_per_tick);
    }

    /* Plot the ticks */
    // Prepare a pen to draw the ticks and memory for the tick labels
    painter.setPen(QPen(QColor(0,0,0)));
    char* chr_buffer  = new char[20];

    // Determine the values of the bottommost and the topmost tick
    double ymin, ymax;
    if (yplot_min == yplot_max) {
        ymin = ymax = yplot_min;
    } else {
        ymin = std::ceil(mult*yplot_min)/mult;
        ymax = std::floor(mult*yplot_max)/mult;
    }

    // Create a lambda to convert x-axis values to horizontal pixel coordinates
    const auto toYCoord = [yplot_min,yplot_max,dy_to_px,y_mid=plot_geom.height()/2](const double y)->int {
        if (yplot_min == yplot_max) {
            assert(y == yplot_min);
            return y_mid;
        } else {
            return (yplot_max-y)*dy_to_px;
        }
    };

    // Iterate through the y-axis values at the tick positions
    double y = ymin;
    int label_count = 0;
    do {
        // Convert the y-axis value to a string
        getLabel(chr_buffer, y);

        // Determine the height of the tick label
        const QRect text_geom   = fm.boundingRect(QString(chr_buffer));
        const int   text_height = text_geom.height();

        // Draw the horizontal tick line
        const int ycoord = toYCoord(y);
        painter.drawLine(geom.width()-tick_width,ycoord,geom.width()-1,ycoord);

        // Draw the tick label
        if (text_height % 2 == 0) {
            // Draw the label only if there's enough space
            if (ycoord-(text_height-2)/2 >= 0 && ycoord+(text_height-2)/2 < geom.height()) {
                // Check if an old tick label can be reused or if a new one must be created
                if (tick_labels_.size() <= label_count) {
                    tick_labels_.push_back(new QLabel(this));
                    tick_labels_.back()->setFont(QFont("Monospace",FONT_SIZE));
                }

                // Configure the label
                tick_labels_[label_count]->setText(QString(chr_buffer));
                tick_labels_[label_count]->setAlignment(Qt::AlignHCenter);
                tick_labels_[label_count]->setGeometry(0,ycoord-(text_height-2)/2,
                                                       text_geom.width()+2*X_MARGIN,text_height);
                tick_labels_[label_count++]->show();
            }
        } else {
            // Draw the label only if there's enough space
            if (ycoord-(text_height-1)/2 >= 0 && ycoord+(text_height-1)/2 < geom.height()) {
                // Check if an old tick label can be reused or if a new one must be created
                if (tick_labels_.size() <= label_count) {
                    tick_labels_.push_back(new QLabel(this));
                    tick_labels_.back()->setFont(QFont("Monospace",FONT_SIZE));
                }

                // Configure the label
                tick_labels_[label_count]->setText(QString(chr_buffer));
                tick_labels_[label_count]->setAlignment(Qt::AlignHCenter);
                tick_labels_[label_count]->setGeometry(0,ycoord-(text_height-1)/2,
                                                       text_geom.width()+2*X_MARGIN,text_height);
                tick_labels_[label_count++]->show();
            }
        }

        // Step to the next y-axis value considered
        y = (y*mult + 1) / mult;
        if (std::abs(y) < 1e-17) {
            y = 0;
        }
    // By increasing y as y->(y*mult+1)/mult, it can happen that y == ymax+epsilon with epsilon>0 and very
    // small. By comparing y to (ymax+0.1/mult) instead of ymax allows epsilon to become as large as 10%
    // of the step values.
    } while (y <= ymax + 0.1/mult);

    // Delete tick labels that are not used anymore
    for (int i = tick_labels_.size()-1; i >= label_count; --i) {
        delete tick_labels_[i];
        tick_labels_.pop_back();
    }

    // Free memory
    delete[] chr_buffer;
}

void VTicksArea::resizeEvent(QResizeEvent* event) {}

uint VTicksArea::getMaxLabelWidth(const uint plot_area_width, const uint plot_area_height) {
    // Miscellaneous
    QFontMetrics fm(QFont("Monospace",FONT_SIZE));

    // Determine the axis limits
    const auto [xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px] =
        limits_info_->getScalingsAndLimits(plot_area_width,plot_area_height);

    /* Determine the tick locations */
    double mult = 1.0;
    // If there are no data points or they are aligned on a horizontal line, just add a single tick at 0 or
    // at the value of the horizontal line, respectively.
    if (yplot_min == yplot_max) {
        // Nothing to do
    }
    // Else, find a good number of ticks such that tick labels don't overlap, but the ticks are sufficiently
    // dense.
    else {
        // Determine the minimum number of pixels per tick
        const uint min_pixel_per_tick = fm.height();

        // Minimize num_pixel_per_tick just below num_pixel_per_tick
        int num_pixel_per_tick = -1;
        const double mult_mult = 2.0;
        double mult10          = 1.0;
        do {
            const int ymin = std::ceil(mult*mult10*yplot_min);
            const int ymax = std::floor(mult*mult10*yplot_max);

            // The y-tick area has the same height as the plot area
            if (ymax-ymin+1 != 0) num_pixel_per_tick = plot_area_height / (ymax-ymin+1);
            mult              *= mult_mult;
            if (mult >= 10.0) {
                mult    = 1.0;
                mult10 *= 10.0;
            }
        } while (num_pixel_per_tick == -1 || num_pixel_per_tick >= min_pixel_per_tick);
        mult = mult*mult10;

        //--> At this point it holds that num_pixel_per_tick < min_pixel_per_tick
        assert(num_pixel_per_tick < min_pixel_per_tick);

        // Decrease mult until 
        while (num_pixel_per_tick < min_pixel_per_tick) {
            mult              /= mult_mult;
            const int ymin     = std::ceil(mult*yplot_min);
            const int ymax     = std::floor(mult*yplot_max);
            num_pixel_per_tick = plot_area_height / (ymax-ymin+1); // The y-tick area has the same height
                                                                   // as the plot area
        }

        //--> At this point it holds that num_pixel_per_tick >= min_pixel_per_tick
        assert(num_pixel_per_tick >= min_pixel_per_tick);
    }

    // Determine the values of the bottommost and the topmost tick
    const double ymin = std::ceil(mult*yplot_min)/mult;
    const double ymax = std::floor(mult*yplot_max)/mult;

    // Iterate through the y-axis values at the tick positions
    char*  chr_buffer     = new char[20];
    int    max_text_width = 0;
    double y              = ymin;
    do {
        // Convert the y-axis value to a string
        getLabel(chr_buffer,y);

        // Update the maximum text width
        max_text_width = std::max(max_text_width,fm.boundingRect(QString(chr_buffer)).width());

        // Step to the next y-axis value considered
        y = (y*mult + 1)/mult;
    } while (y <= ymax);

    // Free memory
    delete[] chr_buffer;

    return max_text_width + 2*getXMargin();
}

void VTicksArea::getLabel(char* chr_buffer, const double y) const {
    // TODO: Put this setting somewhere else
    const uint num_digits = 5;

    if (y == 0.0) {
        sprintf(chr_buffer,"0\n");
    } else {
        const double exp = std::log10(std::abs(y));
        if (exp < num_digits && exp >= 0.0) {
            sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-static_cast<uint>(std::floor(exp))-1)+"g\n").c_str(),static_cast<double>(y));
        } else if (exp < 0.0 && std::abs(exp) < num_digits-1) {
            sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"g\n").c_str(),static_cast<double>(y));
        } else {
            sprintf(chr_buffer,std::string("%."+std::to_string(num_digits-1)+"e\n").c_str(),static_cast<double>(y));
        }
    }
}

};
