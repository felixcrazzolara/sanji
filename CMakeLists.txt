cmake_minimum_required(VERSION 3.20.0)
project(simulation_project)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "-O2")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
if (NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
endif()

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
    include/PlotUI.hpp
    src/PlotUI.cpp
)

macro(add_example _target)
    add_executable(${_target}
        examples/${_target}.cpp
    )
    target_link_libraries(${_target}
        PRIVATE sanji
    )
endmacro()

add_example(SimpleLinePlot)
add_example(Quiver)
add_example(HeatQuiver)
add_example(DotPlot)

target_include_directories(sanji
    PUBLIC include
)

target_link_libraries(sanji
    PUBLIC  Eigen3::Eigen
    PUBLIC  Qt::Widgets
)
