#include "Colors.hpp"
#include "Sanji.hpp"

#include <cmath>

using namespace sanji::colors;

/* Type definitions */
using VectorXd = Eigen::VectorXd;
using MatrixXd = Eigen::MatrixXd;

void simpleLinePlot(int argc, char* argv[]) {
    // Initialize this as a Qt application
    QApplication app(argc, argv);
    sanji::init();

    // Create a curve
    const uint Nx = 1000;
    VectorXd x(Nx);
    VectorXd y1(Nx); 
    VectorXd y2(Nx); 
    for (uint i = 0; i < Nx; ++i) {
        x(i)  = i*2.0*M_PI/(Nx-1);
        y1(i) = std::sin(x(i));
        y2(i) = std::cos(x(i));
    }

    // Plot the data
    sanji::figure("Simple data");
    sanji::plot(x, y1, {{"line_style", '-'}, {"color", RED}}, 10);
    sanji::plot(x, y2, {{"line_style", '-'}, {"color", BLUE}}, 2);

    sanji::figure("Other data");
    VectorXd x_(2);
    x_ << 0, 6;
    VectorXd y_(2);
    y_ << -2, -2;
    sanji::plot(x_,y_);
    y_ << -2.2, -2.2;
    sanji::plot(x_,y_);

    // Execute the application
    app.exec();
}
