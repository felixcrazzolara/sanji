cmake_minimum_required(VERSION 3.20.0)
project(simulation_project)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

#set(CMAKE_CXX_FLAGS "-O3")

set(Qt5Core_DIR "/home/felix/scientific-software/Qt/5.15.2/gcc_64/lib/cmake/Qt5Core")
set(Qt5_DIR     "/home/felix/scientific-software/Qt/5.15.2/gcc_64/lib/cmake/Qt5")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 5.15.2 COMPONENTS Widgets REQUIRED)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

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
)

add_executable(SimpleLinePlot
    examples/SimpleLinePlot.cpp
)

add_executable(Quiver
    examples/Quiver.cpp
)

add_executable(HeatQuiver
    examples/HeatQuiver.cpp
)

target_include_directories(sanji
    PUBLIC include
)

target_link_libraries(SimpleLinePlot
    PRIVATE sanji
)

target_link_libraries(Quiver
    PRIVATE sanji
)

target_link_libraries(HeatQuiver
    PRIVATE sanji
)

target_link_libraries(sanji
    PUBLIC Eigen3::Eigen
    PUBLIC Qt5::Widgets
)
