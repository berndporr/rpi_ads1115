cmake_minimum_required(VERSION 3.1.0)
add_executable (ads1115_printer ads1115_printer.cpp)
target_link_libraries(ads1115_printer ads1115rpi_static)
target_include_directories(ads1115_printer PRIVATE ..)
