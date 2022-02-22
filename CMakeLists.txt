cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

project(ADS1115_RaspberryPi_Library VERSION 1.0.0 LANGUAGES CXX)
include(GNUInstallDirs)

add_subdirectory(example)

add_compile_options(-Wall -Wconversion -Wextra -pedantic)

set(LIBSRC ads1115rpi.cpp)
set(LIBINCLUDE ads1115rpi.h)

add_library(ads1115rpi
  SHARED
  ${LIBSRC}
  )

set_target_properties(ads1115rpi PROPERTIES
  SOVERSION 1
  PUBLIC_HEADER "${LIBINCLUDE}")

target_link_libraries(ads1115rpi pigpio)

install(TARGETS ads1115rpi EXPORT ads1115rpi-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})



add_library(ads1115rpi_static
  STATIC
  ${LIBSRC}
  )

set_target_properties(ads1115rpi_static PROPERTIES
  PUBLIC_HEADER "${LIBINCLUDE}")

target_link_libraries(ads1115rpi_static pigpio)

install(TARGETS ads1115rpi_static EXPORT ads1115rpi_static-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
