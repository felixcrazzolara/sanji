#include "Sanji.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstring>

TEST_CASE("Quiver") {
    // Initialize this as a Qt application
    int argc = 1;
    char *app_name = new char[100];
    std::strcpy(app_name, "Test");
    QApplication app(argc, &app_name);
    sanji::init();
    delete[] app_name;
}
