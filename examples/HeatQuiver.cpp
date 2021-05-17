#include <cmath>
#include "Sanji.hpp"
#include "Colors.hpp"

using namespace sanji::colors;

/* Type definitions */
using VectorXd = Eigen::VectorXd;
using MatrixXd = Eigen::MatrixXd;

int main(int argc, char* argv[]) {
    // Initialize this as a Qt application
    QApplication app(argc, argv);
    sanji::init();

    // Create a curve
    uint Nx = 10;
    VectorXd x(Nx*Nx);
    VectorXd y(Nx*Nx);
    VectorXd u(Nx*Nx);
    VectorXd v(Nx*Nx);
    uint cnt = 1;
    for (uint x_ = 0; x_ < Nx; ++x_) {
        for (uint y_ = 0; y_ < Nx; ++y_) {
            x(cnt-1)  = x_;
            y(cnt-1)  = y_;
            u(cnt-1)  = cnt;
            v(cnt-1)  = 0.0;
            ++cnt;
        }
    }

    // Plot the data
    sanji::figure("Simple data");
    sanji::quiver(x,y,u,v,{{"arrow_length",0.5},{"use_colormap",1},{"colormap",TURBO},{"min",1},{"max",Nx*Nx}});
    sanji::setPlotBackgroundColor(BLACK);
    sanji::setAxesRatio("equal");

    // Execute the application
    app.exec();
}
