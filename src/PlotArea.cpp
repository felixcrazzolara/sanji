#include <QPainter>
#include <cmath>
#include <complex>
#include "PlotArea.hpp"
#include "Figure.hpp"

#include <iostream> // TODO: Remove this later

namespace sanji_ {

using namespace std::complex_literals;

/* Type definitions */
using complex = std::complex<double>;

PlotArea::PlotArea(const vector<tuple<uint,vec_ptr,mat_ptr,QPen>>*                   line_data,
                   const vector<tuple<uint,vec_ptr,vec_ptr,vec_ptr,vec_ptr,QBrush>>* arrow_data,
                   const LimitsInfo*                                                 limits_info) :
    line_data_(line_data),
    arrow_data_(arrow_data),
    limits_info_(limits_info)
{}

void PlotArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Fetch the geometry of the this widget
    const QRect geom = geometry();

    // Create a lambda to convert x-y-coordinates to pixel coordinates
    const auto toQPoint = [xmin=limits_info_->xmin,
                           ymax=limits_info_->ymax,
                           x_to_px=geom.width()/(limits_info_->xmax-limits_info_->xmin),
                           y_to_px=geom.height()/(limits_info_->ymax-limits_info_->ymin)](const double x, const double y)->QPoint {
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

    // Plot the arrow data
    painter.setPen(Qt::NoPen);
    const double    tail_width  = 0.2;
    const double    tail_length = 0.7;
    vector<complex> base_tail_corners{           -0.5i*tail_width,
                                      tail_length-0.5i*tail_width,
                                      tail_length+0.5i*tail_width,
                                                  0.5i*tail_width};
    const double    head_width  = 0.4;
    const double    head_length = 0.3;
    vector<complex> base_head_corners{-0.5i*head_width,
                                      head_length,
                                      0.5i*head_width};
    for (const auto& arrow_data : *arrow_data_) {
        // Extract the variables for convenience
        const VectorXd& x = *std::get<1>(arrow_data);
        const VectorXd& y = *std::get<2>(arrow_data);
        const VectorXd& u = *std::get<3>(arrow_data);
        const VectorXd& v = *std::get<4>(arrow_data);

        // Configure the painter
        painter.setBrush(std::get<5>(arrow_data));

        // Draw the data
        for (uint i = 0; i < x.rows(); ++i) {
            const double phi             = atan2(v(i),u(i));
            const double arrow_length    = std::sqrt(v(i)*v(i)+u(i)*u(i));
            const complex rot            = std::cos(phi)+1.0i*std::sin(phi);
            vector<complex> tail_corners = base_tail_corners;
            for (auto& corner : tail_corners) corner *= rot*arrow_length;
            vector<complex> head_corners = base_head_corners;
            for (auto& corner : head_corners) corner *= rot*arrow_length;

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

};
