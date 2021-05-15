#include "RenderArea.hpp"
#include <QPainter>
#include "Figure.hpp"

#include <QDebug> // TODO: Remove this later

#include <iostream> // TODO: Remove this later

namespace sanji_ {

RenderArea::RenderArea(QWidget* parent, const vector<tuple<uint,vec_ptr,mat_ptr,QPen>>* data, const LimitsInfo* limits_info) :
    QWidget(parent),
    limits_info_(limits_info),
    data_(data)
{
    this->show();
}

void RenderArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // Plot the data
    for (const auto& data : *data_) {
        // Extract variables for convenience
        const VectorXd& x = *std::get<1>(data);
        const MatrixXd& y = *std::get<2>(data);

        // Configure the painter
        painter.setPen(std::get<3>(data));

        if (x.rows() == 0) continue;

        // Create a lambda to convert x-y-coordinates to pixel coordinates
        const auto toQPoint = [xmin=limits_info_->xmin,
                               ymax=limits_info_->ymax,
                               x_to_px=geometry().width()/(limits_info_->xmax-limits_info_->xmin),
                               y_to_px=geometry().height()/(limits_info_->ymax-limits_info_->ymin)](const double x, const double y)->QPoint {
            return QPoint((x-xmin)*x_to_px,(ymax-y)*y_to_px);
        };

        if (x.rows() == 1) {
            std::cout << "This is currently not implemented (from RenderArea::paintEvent)" << std::endl; // TODO: Implement this
        } else
            for (uint i = 0; i < x.rows()-1; ++i)
                for (uint j = 0; j < y.cols(); ++j)
                    painter.drawLine(toQPoint(x(i),y(i,j)),toQPoint(x(i+1),y(i+1,j)));
    }
}

};
