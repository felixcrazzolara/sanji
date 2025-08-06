cmake_minimum_required(VERSION 3.20.0)
project(sanji)

set(CMAKE_CXX_FLAGS "-O2 -std=c++2b")
#set(CMAKE_CXX_FLAGS "-g -std=c++2b")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Core)
if (NOT Qt6_FOUND)
    message("Failed to find Qt6, falling back to Qt5.")
    find_package(Qt5 REQUIRED COMPONENTS Widgets Core)
endif()

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

set(BUILD_MATH_TESTS OFF CACHE BOOL "Whether to build the tests of the math framework.")
add_subdirectory(3rdparty/math)

if (NOT BUILD_MATH_TESTS)
    add_subdirectory(3rdparty/Catch2)
endif()

add_library(sanji SHARED
    src/Sanji.cpp
    src/Figure.cpp
    include/Figure.hpp
    src/RenderArea.cpp
    include/RenderArea.hpp
    src/PlotArea.cpp
    include/PlotArea.hpp
    src/HTicksArea.cpp
    include/HTicksArea.hpp
    src/VTicksArea.cpp
    include/VTicksArea.hpp
    src/Colors.cpp
    include/TicksArea.hpp
    src/TicksArea.cpp
    src/Colormaps.cpp
    include/PlotUI.hpp
    src/PlotUI.cpp
)

add_library(examples
    examples/DotPlot.cpp
    examples/HeatQuiver.cpp
    examples/Quiver.cpp
    examples/ShowImage.cpp
    examples/SimpleLinePlot.cpp
)

target_compile_definitions(examples
    PRIVATE TEST_IMG_DIR="${CMAKE_CURRENT_SOURCE_DIR}/img/"
)

target_link_libraries(examples
    PUBLIC sanji
)

add_executable(example
    examples/example.cpp
)

target_link_libraries(example
    PUBLIC examples
)

target_include_directories(sanji
    PUBLIC include
)

target_link_libraries(sanji
    PUBLIC Eigen3::Eigen
    PUBLIC math
)
if (Qt6_FOUND)
    target_link_libraries(sanji
        PUBLIC Qt6::Widgets
        PUBLIC Qt6::Core
    )
else()
    target_link_libraries(sanji
        PUBLIC Qt5::Widgets
        PUBLIC Qt5::Core
    )
endif()

add_executable(tests
    tests/tests.cpp
)

target_link_libraries(tests
    PRIVATE Catch2::Catch2WithMain
    PRIVATE sanji
)

if (NOT BUILD_MATH_TESTS)
    include(CTest)
    include(Catch)
endif()
catch_discover_tests(tests)
