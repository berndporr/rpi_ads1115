cmake_minimum_required(VERSION 3.20.0)

project(adc-plot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(ads1115plotter
  window.cpp
  main.cpp
)

target_link_libraries(ads1115plotter Qt6::Widgets)
target_link_libraries(ads1115plotter QCustomPlotQt6)
target_link_libraries(ads1115plotter ads1115rpi)
