#include <QPainter>
#include <cmath>
#include <complex>
#include "PlotArea.hpp"
#include "Figure.hpp"
#include "Colors.hpp"

#include <iostream> // TODO: Remove this later

namespace sanji_ {

/* Namespaces */
using namespace sanji::colors;
using namespace std::complex_literals;

/* Type definitions */
using complex = std::complex<double>;

PlotArea::PlotArea(const vector<tuple<uint,vec_ptr,mat_ptr,QPen>>*                  line_data,
                   const vector<tuple<uint,vec_ptr,vec_ptr,vec_ptr,vec_ptr,Style>>* arrow_data,
                   const LimitsInfo*                                                limits_info) :
    background_color_{255,255,255},
    line_data_(line_data),
    arrow_data_(arrow_data),
    limits_info_(limits_info)
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
    double xmin;
    double ymax;
    double x_to_px = geom.width()/(limits_info_->xmax-limits_info_->xmin);
    double y_to_px = geom.height()/(limits_info_->ymax-limits_info_->ymin);
    if (limits_info_->axes_ratio == LimitsInfo::AXES_RATIO::EQUAL) {
        if (x_to_px >= y_to_px) {
            x_to_px = y_to_px;
            ymax    = limits_info_->ymax;
            xmin    = limits_info_->xmin-std::max(geom.width()/x_to_px-(limits_info_->xmax-limits_info_->xmin),0.0)/2.0;
        } else {
            y_to_px = x_to_px;
            xmin    = limits_info_->xmin;
            ymax    = limits_info_->ymax+std::max(geom.height()/y_to_px-(limits_info_->ymax-limits_info_->ymin),0.0)/2.0;
        }
    } else {
        xmin    = limits_info_->xmin;
        ymax    = limits_info_->ymax;
    }
    const auto toQPoint = [xmin,ymax,x_to_px,y_to_px](const double x, const double y)->QPoint {
        return QPoint((x-xmin)*x_to_px,(ymax-y)*y_to_px);
    };

    // Plot the line data
    for (const auto& line_data : *line_data_) {
        // Extract variables for convenience
        const VectorXd& x = *std::get<1>(line_data);
        const MatrixXd& y = *std::get<2>(line_data);

        // Configure the painter
        painter.setPen(std::get<3>(line_data));

        // Draw the data
        if (x.rows() == 1) {
            std::cout << "This is currently not implemented (from RenderArea::paintEvent)" << std::endl; // TODO: Implement this
        } else
            for (uint i = 0; i < x.rows()-1; ++i)
                for (uint j = 0; j < y.cols(); ++j)
                    painter.drawLine(toQPoint(x(i),y(i,j)),toQPoint(x(i+1),y(i+1,j)));
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
    for (const auto& arrow_data : *arrow_data_) {
        // Extract the variables for convenience
        const VectorXd& x     = *std::get<1>(arrow_data);
        const VectorXd& y     = *std::get<2>(arrow_data);
        const VectorXd& u     = *std::get<3>(arrow_data);
        const VectorXd& v     = *std::get<4>(arrow_data);
        const Style&    style =  std::get<5>(arrow_data);

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
            const double arrow_length    = use_colormap ? style.at("arrow_length") : data_length;

            // Compute the coordinates of the scaled and rotated arrow
            const double phi             = std::atan2(v(i),u(i));
            const complex rot            = std::cos(phi)+1.0i*std::sin(phi);
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

            const QPoint head_points[3] = {
                toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[0]).real(),y(i)+(rot*tail_length*arrow_length+head_corners[0]).imag()),
                toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[1]).real(),y(i)+(rot*tail_length*arrow_length+head_corners[1]).imag()),
                toQPoint(x(i)+(rot*tail_length*arrow_length+head_corners[2]).real(),y(i)+(rot*tail_length*arrow_length+head_corners[2]).imag())
            };
            painter.drawConvexPolygon(head_points,3);

            const QPoint tail_points[4] = {
                toQPoint(x(i)+tail_corners[0].real(),y(i)+tail_corners[0].imag()),
                toQPoint(x(i)+tail_corners[1].real(),y(i)+tail_corners[1].imag()),
                toQPoint(x(i)+tail_corners[2].real(),y(i)+tail_corners[2].imag()),
                toQPoint(x(i)+tail_corners[3].real(),y(i)+tail_corners[3].imag())
            };
            painter.drawConvexPolygon(tail_points,4);
        }
    }
}

void PlotArea::setBackgroundColor(const uint32_t color) {
    background_color_ = QColor(0xffu&(color>>16),0xffu&(color>>8),0xffu&color);
}

};
