#include "Sanji.hpp"

#include "Image.hpp"

void showImage(int argc, char* argv[]) {
    // Initialize this as a Qt application
    QApplication app(argc, argv);
    sanji::init();

    // Load the image
    Image img = Image::load(std::string{TEST_IMG_DIR} + "/red.png");

    // Show the image
    sanji::figure("SinePlot.jpg");
    sanji::imshow(img);

    // Execute the application
    app.exec();
}
