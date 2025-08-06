#include <iostream>

// Forward declarations
void heatQuiver(int argc, char* argv[]);
void dotPlot(int argc, char* argv[]);
void quiver(int argc, char* argv[]);
void showImage(int argc, char* argv[]);
void simpleLinePlot(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    std::cout << "Which example to you want to try?\n";
    std::cout << "[1] HeatQuiver\n";
    std::cout << "[2] DotPlot\n";
    std::cout << "[3] Quiver\n";
    std::cout << "[4] ShowImage\n";
    std::cout << "[5] SimpleLinePlot" << std::endl;
    int input;
    std::cin >> input;
    if (input == 1) {
        heatQuiver(argc, argv);
    } else if (input == 2) {
        dotPlot(argc, argv);
    } else if (input == 3) {
        quiver(argc, argv);
    } else if (input == 4) {
        showImage(argc, argv);
    } else if (input == 5) {
        simpleLinePlot(argc, argv);
    }
}
