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

    // Define the data
    VectorXd x = VectorXd::LinSpaced(10,0,9);
    VectorXd y = VectorXd::LinSpaced(10,0.1,1);

    // Plot the data
    sanji::figure("Dot plot example");
    sanji::plot(x,y,{{"line_style",'o'}});

    // Execute the application
    app.exec();
}
