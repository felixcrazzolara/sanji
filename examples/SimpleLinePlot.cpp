#include <cmath>
#include "Sanji.hpp"
#include "Colors.hpp"

using namespace sanji::colors;

/* Type definitions */
using VectorXd = Eigen::VectorXd;
using MatrixXd = Eigen::MatrixXd;

int main(int argc, char* argv[]) {
    // Initialize this as a QT application
    QApplication app(argc, argv);
    sanji::init();

    // Create a curve
    uint Nx = 1000;
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
    sanji::plot(x,y1,10,'-',RED);
    sanji::plot(x,y2,2,'-',BLUE);

    // Execute the application
    app.exec();
}
