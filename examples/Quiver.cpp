#include "Colors.hpp"
#include "Sanji.hpp"

#include <cmath>

using namespace sanji::colors;

/* Type definitions */
using VectorXd = Eigen::VectorXd;
using MatrixXd = Eigen::MatrixXd;

void quiver(int argc, char* argv[]) {
    // Initialize this as a Qt application
    QApplication app(argc, argv);
    sanji::init();

    // Create a curve
    const uint Nx = 1;
    VectorXd x(Nx);
    VectorXd y(Nx);
    VectorXd u(Nx);
    VectorXd v(Nx);
    for (uint i = 0; i < Nx; ++i) {
        x(i)  = std::cos(i*2.0*M_PI/Nx);
        y(i)  = std::sin(i*2.0*M_PI/Nx);
        u(i)  = 0.1*std::cos(i*2.0*M_PI/Nx);
        v(i)  = 0.1*std::sin(i*2.0*M_PI/Nx);
    }

    // Plot the data
    sanji::figure("Simple data");
    sanji::quiver(x,y,u,v,{{"line_style",'-'},{"color",RED}},10);
    sanji::setAxesRatio("equal");

    // Execute the application
    app.exec();
}
