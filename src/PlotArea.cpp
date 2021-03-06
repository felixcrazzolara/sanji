#include <QPainter>

#include <cmath>
#include <complex>

#include "PlotArea.hpp"
#include "Figure.hpp"
#include "Colors.hpp"

#include <iostream>

namespace sanji_ {

/* Namespaces */
using namespace sanji::colors;
using namespace std::complex_literals;

/* Type definitions */
using complex = std::complex<double>;

PlotArea::PlotArea(const LineDataWrapper*  line_data,
                   const ArrowDataWrapper* arrow_data,
                         LimitsInfo*       limits_info,
                         QWidget*          parent) :
    QWidget(parent),
    background_color_{255,255,255},
    line_data_(line_data),
    arrow_data_(arrow_data),
    limits_info_(limits_info),
    selection_active_{false}
{}

void PlotArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

    // Fill the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color_);
    painter.drawRect(0,0,geom.width(),geom.height());

    // Create a lambda to convert x-y-coordinates to pixel coordinates
    const QRect plot_geom = geometry();
    const auto [xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px] =
        limits_info_->getScalingsAndLimits(plot_geom.width(),plot_geom.height());
    const auto toQPoint = [xplot_min,xplot_max,yplot_min,yplot_max,dx_to_px,dy_to_px,x_mid=geom.width()/2,
        y_mid=geom.height()/2](const double x, const double y, const double use_fixed_scaling=false,
                               const double dx_to_px_f=0.0, const double dy_to_px_f=0.0)->QPoint {
        if (use_fixed_scaling) {
            return QPoint(x_mid+(x-xplot_min)*dx_to_px_f,y_mid+(yplot_max-y)*dy_to_px_f);
        } else {
            uint x_px, y_px;

            if (xplot_min == xplot_max) {
                assert(x == xplot_min);
                x_px = x_mid;
            } else {
                x_px = (x-xplot_min)*dx_to_px;
            }
            if (yplot_min == yplot_max) {
                assert(y == yplot_min);
                y_px = y_mid;
            } else {
                y_px = (yplot_max-y)*dy_to_px;
            }

            return QPoint(x_px,y_px);
        }
    };

    // Plot the line data
    for (const auto& line_data : line_data_->c_data()) {
        // Extract variables for convenience
        const VectorXd& x     = *line_data.x;
        const MatrixXd& y     = *line_data.y;
        const Style&    style =  line_data.style;

        // Configure the painting color
        uint32_t color;
        if (style.find("color") == style.end()) color = BLACK;
        else                                    color = style.at("color");

        if (style.find("line_style") != style.end() && style.at("line_style") == 'o') {
            // Configure the painter
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff)));

            // Draw the data
            if (x.rows() == 1) {
                std::cout << "This is currently not implemented (from RenderArea::paintEvent)" << std::endl; // TODO: Implement this
            } else
                for (uint i = 0; i < x.rows(); ++i)
                    for (uint j = 0; j < y.cols(); ++j)
                        painter.drawEllipse(toQPoint(x(i),y(i,j)),4,4);
        } else {
            // Configure the pen
            QPen pen(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff));
            if (style.find("line_style") == style.end()) pen.setStyle(Qt::SolidLine);
            else {
                if (style.at("line_style") == '-')      pen.setStyle(Qt::SolidLine);
                else if (style.at("line_style") == '.') pen.setStyle(Qt::DotLine);
                else {
                    std::cout << "'"+std::to_string((char) style.at("line_style"))+"' is not a valid 'line_style'." << std::endl;
                    abort();
                }
            }
            painter.setPen(pen);

            // Draw the data
            if (x.rows() == 1) {
                std::cout << "This is currently not implemented (from RenderArea::paintEvent)" << std::endl; // TODO: Implement this
            } else
                for (uint i = 0; i < x.rows()-1; ++i)
                    for (uint j = 0; j < y.cols(); ++j)
                        painter.drawLine(toQPoint(x(i),y(i,j)),toQPoint(x(i+1),y(i+1,j)));
        }
    }

    /* Plot the arrow data */
    painter.setPen(Qt::NoPen);

    // Define the unit arrow
    const double          tail_width  = 0.2;
    const double          tail_length = 0.7;
    const vector<complex> base_tail_corners{           -0.5i*tail_width,
                                      tail_length-0.5i*tail_width,
                                      tail_length+0.5i*tail_width,
                                                  0.5i*tail_width};
    const double          head_width  = 0.4;
    const double          head_length = 0.3;
    const vector<complex> base_head_corners{-0.5i*head_width,
                                      head_length,
                                      0.5i*head_width};

    // Iterate through the data
    for (const auto& arrow_data : arrow_data_->c_data()) {
        // Extract the variables for convenience
        const VectorXd& x     = *arrow_data.x;
        const VectorXd& y     = *arrow_data.y;
        const VectorXd& u     = *arrow_data.u;
        const VectorXd& v     = *arrow_data.v;
        const Style&    style =  arrow_data.style;

        /* Configure the brush */
        bool use_colormap          = false;
        bool heatmap_use_linscale  = true;
        double min,max;
        if (style.find("use_colormap") == style.end()) {
            // Color
            uint32_t color;
            if (style.find("color") == style.end()) color = BLACK;
            else                                    color = style.at("color");
            painter.setBrush(QBrush(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff)));
        } else {
            // Check the arguments
            if (style.find("colormap") == style.end()) throw new std::runtime_error("Must provided argument 'colormap' when using the option 'use_colormap' for 'quiver'.");
            if (style.find("min") == style.end())      throw new std::runtime_error("Must provided argument 'min' when using the option 'use_colormap' for 'quiver'.");
            if (style.find("max") == style.end())      throw new std::runtime_error("Must provided argument 'max' when using the option 'use_colormap' for 'quiver'.");
            if (style.at("colormap") != TURBO)         throw new std::runtime_error("Currently 'quiver' supports only the colormap 'TURBO'.");

            // Parse the arguments and set the flag
            if (style.find("use_logscale") != style.end()) heatmap_use_linscale = false;
            min = style.at("min");
            max = style.at("max");
            use_colormap = true;
        }

        // Draw the data
        for (uint i = 0; i < x.rows(); ++i) {
            // Define the arrow length
            const double data_length     = std::sqrt(v(i)*v(i)+u(i)*u(i));
            const double arrow_length    = use_colormap || style.find("arrow_length") != style.end() ? style.at("arrow_length") : data_length;

            // Compute the coordinates of the scaled and rotated arrow
            const double    phi          = std::atan2(v(i),u(i));
            const complex   rot          = std::cos(phi)+1.0i*std::sin(phi);
            vector<complex> tail_corners = base_tail_corners;
            vector<complex> head_corners = base_head_corners;
            if (style.find("center_arrows") != style.end()) {
                for (complex& c : tail_corners) c -= 0.5;
                for (complex& c : head_corners) c -= 0.5;
            }
            for (auto& corner : tail_corners) corner *= rot*arrow_length;
            for (auto& corner : head_corners) corner *= rot*arrow_length;

            // Possibly configure the brush
            if (use_colormap) {
                if (heatmap_use_linscale)
                    painter.setBrush(QBrush(QColor(to_turbo_rgb((data_length-min)/(max-min)))));
                else
                    painter.setBrush(QBrush(QColor(to_turbo_rgb((std::log10(data_length)-std::log10(min))/(std::log10(max)-std::log10(min))))));
            }

            if (x.rows() > 1) {
                const QPoint head_points[3] = {
                    toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[0]).real(),
                             y(i)+(rot*tail_length*arrow_length+head_corners[0]).imag()),
                    toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[1]).real(),
                             y(i)+(rot*tail_length*arrow_length+head_corners[1]).imag()),
                    toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[2]).real(),
                             y(i)+(rot*tail_length*arrow_length+head_corners[2]).imag())
                };
                painter.drawConvexPolygon(head_points,3);

                const QPoint tail_points[4] = {
                    toQPoint(x(i)+tail_corners[0].real(),y(i)+tail_corners[0].imag()),
                    toQPoint(x(i)+tail_corners[1].real(),y(i)+tail_corners[1].imag()),
                    toQPoint(x(i)+tail_corners[2].real(),y(i)+tail_corners[2].imag()),
                    toQPoint(x(i)+tail_corners[3].real(),y(i)+tail_corners[3].imag())
                };
                painter.drawConvexPolygon(tail_points,4);
            } else {
                const double dxy_to_px = std::min(0.2*geom.height()/arrow_length,
                                                  0.2*geom.width()/arrow_length);

                const QPoint head_points[3] = {
                    toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[0]).real(),
                             y(i)+(rot*tail_length*arrow_length+head_corners[0]).imag(),
                             true,
                             dxy_to_px,
                             dxy_to_px),
                    toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[1]).real(),
                             y(i)+(rot*tail_length*arrow_length+head_corners[1]).imag(),
                             true,
                             dxy_to_px,
                             dxy_to_px),
                    toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[2]).real(),
                             y(i)+(rot*tail_length*arrow_length+head_corners[2]).imag(),
                             true,
                             dxy_to_px,
                             dxy_to_px)
                };
                painter.drawConvexPolygon(head_points,3);

                const QPoint tail_points[4] = {
                    toQPoint(x(i)+tail_corners[0].real(),
                             y(i)+tail_corners[0].imag(),
                             true,
                             dxy_to_px,
                             dxy_to_px),
                    toQPoint(x(i)+tail_corners[1].real(),
                             y(i)+tail_corners[1].imag(),
                             true,
                             dxy_to_px,
                             dxy_to_px),
                    toQPoint(x(i)+tail_corners[2].real(),
                             y(i)+tail_corners[2].imag(),
                             true,
                             dxy_to_px,
                             dxy_to_px),
                    toQPoint(x(i)+tail_corners[3].real(),
                             y(i)+tail_corners[3].imag(),
                             true,
                             dxy_to_px,
                             dxy_to_px)
                };
                painter.drawConvexPolygon(tail_points,4);
            }
        }
    }

    // Draw the selection rectangle (if active)
    if (selection_active_) {
        // Chose the color such that it is always visible on the background
        int r,g,b;
        background_color_.getRgb(&r,&g,&b);
        r = r > 127 ? r - 128 : r + 128;
        g = g > 127 ? g - 128 : g + 128;
        b = b > 127 ? b - 128 : b + 128;
        QPen pen{QColor(r, g, b)}; pen.setWidth(1);
        painter.setPen(pen);

        painter.drawLine(selection_start_.x(),selection_start_.y(),selection_start_.x(),selection_end_.y());
        painter.drawLine(selection_start_.x(),selection_end_.y(),selection_end_.x(),selection_end_.y());
        painter.drawLine(selection_end_.x(),selection_end_.y(),selection_end_.x(),selection_start_.y());
        painter.drawLine(selection_end_.x(),selection_start_.y(),selection_start_.x(),selection_start_.y());
    }
}

void PlotArea::mousePressEvent(QMouseEvent *event) {
    // Remember the start of the selection and activate it
    selection_start_  = event->pos();
    selection_active_ = true;
}

void PlotArea::mouseReleaseEvent(QMouseEvent *event) {
    // Store the end of the selection and deactivate it
    selection_end_    = event->pos();
    selection_active_ = false;

    /* Update the axes limits */
    const uint  diff_px_x = std::abs(selection_start_.x() - selection_end_.x());
    const uint  diff_px_y = std::abs(selection_start_.y() - selection_end_.y());
    const QRect geom      = geometry();
    if (diff_px_x > 0 && diff_px_y > 0) {
        // Compute the plot area limits
        double xmin;
        double ymax;
        double x_to_px = geom.width()/(limits_info_->xmax()-limits_info_->xmin());
        double y_to_px = geom.height()/(limits_info_->ymax()-limits_info_->ymin());
        if (limits_info_->axes_ratio == LimitsInfo::AXES_RATIO::EQUAL) {
            if (x_to_px >= y_to_px) {
                x_to_px = y_to_px;
                ymax    = limits_info_->ymax();
                xmin    = limits_info_->xmin()-std::max(geom.width()/x_to_px-(limits_info_->xmax()-limits_info_->xmin()),0.0)/2.0;
            } else {
                y_to_px = x_to_px;
                xmin    = limits_info_->xmin();
                ymax    = limits_info_->ymax()+std::max(geom.height()/y_to_px-(limits_info_->ymax()-limits_info_->ymin()),0.0)/2.0;
            }
        } else {
            xmin    = limits_info_->xmin();
            ymax    = limits_info_->ymax();
        }

        // Compute the coordinates of the selection
        const double px_x_sel_low  = std::min(selection_start_.x(),selection_end_.x());
        const double px_x_sel_high = std::max(selection_start_.x(),selection_end_.x());
        const double px_y_sel_low  = std::min(selection_start_.y(),selection_end_.y());
        const double px_y_sel_high = std::max(selection_start_.y(),selection_end_.y());
        const double x_sel_low     = xmin + px_x_sel_low/x_to_px;
        const double x_sel_high    = xmin + px_x_sel_high/x_to_px;
        const double y_sel_low     = ymax - px_y_sel_high/y_to_px;
        const double y_sel_high    = ymax - px_y_sel_low/y_to_px;

        if (limits_info_->axes_ratio == LimitsInfo::AXES_RATIO::EQUAL) {
            double x_to_px = diff_px_x/(x_sel_high-x_sel_low);
            double y_to_px = diff_px_y/(y_sel_high-y_sel_low);
            if (x_to_px >= y_to_px) {
                x_to_px = y_to_px;
                const double x_mean = (x_sel_low+x_sel_high)/2.0;
                limits_info_->add_axes_limits(x_mean-0.5*diff_px_x/x_to_px,x_mean+0.5*diff_px_x/x_to_px,y_sel_low,y_sel_high);
            } else {
                y_to_px = x_to_px;
                const double y_mean = (y_sel_low+y_sel_high)/2.0;
                limits_info_->add_axes_limits(x_sel_low,x_sel_high,y_mean-0.5*diff_px_y/y_to_px,y_mean+0.5*diff_px_y/y_to_px);
            }
        } else {
            limits_info_->add_axes_limits(x_sel_low,x_sel_high,y_sel_low,y_sel_high);
        }

        // Update the entire plot
        parentWidget()->update();
    }
}

void PlotArea::mouseMoveEvent(QMouseEvent *event) {
    // Store the current end of the selection and update the graph
    selection_end_ = event->pos();
    update();
}

void PlotArea::setBackgroundColor(const uint32_t color) {
    background_color_ = QColor(0xffu&(color>>16),0xffu&(color>>8),0xffu&color);
}

void PlotArea::setSizeHint(const QSize& size_hint) {
    size_hint_ = size_hint;
}

QSize PlotArea::sizeHint() const {
    return size_hint_;
}

};
